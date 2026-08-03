#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include "hello_ioctl.h"

#define DEVICE_NAME "hellochar"
#define BUF_SIZE 256

static DEFINE_MUTEX(hello_mutex);

static char device_buf[BUF_SIZE];
static int buf_len;

static ssize_t hello_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset) {
    int ret;
    size_t bytes_to_copy;

    mutex_lock(&hello_mutex);
    if (*offset >= buf_len) {
        mutex_unlock(&hello_mutex);
        return 0;
    }

    size_t remaining = buf_len - *offset;
    bytes_to_copy = (count < remaining) ? count : remaining;
    pr_info("%s: read() called, count=%zu, offset=%lld, remaining=%zu, bytes_to_copy=%zu\n", DEVICE_NAME, count, *offset, remaining, bytes_to_copy);

    ret = copy_to_user(user_buf, device_buf + *offset, bytes_to_copy);
    if (ret != 0) {
        mutex_unlock(&hello_mutex);
        return -EFAULT;
    }
    *offset = *offset + bytes_to_copy;

    mutex_unlock(&hello_mutex);
    return bytes_to_copy;
}

static ssize_t hello_write(struct file *file, const char __user *user_buf, size_t count, loff_t *offset)
{
    int ret;
    size_t bytes_to_copy;
    mutex_lock(&hello_mutex);

    if (count > BUF_SIZE) {
        bytes_to_copy = BUF_SIZE;
        pr_info("%s: WARNING: write() called, count=%zu, bytes_to_copy=%zu, dropped_bytes=%zu\n", DEVICE_NAME, count, bytes_to_copy, count - bytes_to_copy);

    } else {
        bytes_to_copy = count;
        pr_info("%s: write() called, count=%zu, bytes_to_copy=%zu\n", DEVICE_NAME, count, bytes_to_copy);
    }


    ret = copy_from_user(device_buf, user_buf, bytes_to_copy);
    if (ret != 0) {
        mutex_unlock(&hello_mutex);
        return -EFAULT;
    }

    buf_len = bytes_to_copy;

    mutex_unlock(&hello_mutex);
    return count;
}

static int hello_open(struct inode *inode, struct file *file) {
    pr_info("%s: called open()\n", DEVICE_NAME);
    return 0;
}

static int hello_release(struct inode *inode, struct file *file) {
    pr_info("%s: called release()\n", DEVICE_NAME);
    return 0;
}

static long hello_ioctl(struct file * file, unsigned int cmd, unsigned long arg) {
    mutex_lock(&hello_mutex);
    switch (cmd) {
        case HELLO_RESET: {
            buf_len = 0;
            break;
        }
        case HELLO_GET_SIZE: {
            int ret = copy_to_user((int __user *)arg, &buf_len, sizeof(buf_len));
            if (ret != 0) {
                mutex_unlock(&hello_mutex);
                return -EFAULT;
            }
            break;
        }
        default:{
            mutex_unlock(&hello_mutex);   
            return -ENOTTY;
        }
    }
    mutex_unlock(&hello_mutex);
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
    .write = hello_write,
    .unlocked_ioctl = hello_ioctl,
};

static struct miscdevice hello_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DEVICE_NAME,
    .fops  = &fops,
};

static int __init hello_init(void)
{
    int ret;
    // TODO: misc_register(&hello_misc) — check return value, same error-handling
    //       instinct as before: what do you return on failure?
    ret = misc_register(&hello_misc);
    if (ret != 0) return ret;
    pr_info("%s: module loaded successfully.\n", DEVICE_NAME);
    return 0;
}

static void __exit hello_exit(void)
{
    // TODO: misc_deregister(&hello_misc)
    misc_deregister(&hello_misc);
    pr_info("%s: module unloaded successfully.\n", DEVICE_NAME);
}


module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("manual char device registration");
