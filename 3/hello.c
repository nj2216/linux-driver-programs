#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEVICE_NAME "hellochar"
#define BUF_SIZE 256

static dev_t dev_num;
static struct cdev my_cdev;
static char device_buf[BUF_SIZE];
static int buf_len;

static ssize_t hello_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset) {
    int ret;
    if (*offset >= buf_len) return 0;

    size_t bytes_to_copy;
    size_t remaining = buf_len - *offset;
    bytes_to_copy = (count < remaining) ? count : remaining;
    pr_info("%s: read() called, count=%zu, offset=%lld, remaining=%zu, bytes_to_copy=%zu\n",
            DEVICE_NAME, count, *offset, remaining, bytes_to_copy);

    ret = copy_to_user(user_buf, device_buf + *offset, bytes_to_copy);
    if (ret != 0) return -EFAULT;
    *offset = *offset + bytes_to_copy;

    return bytes_to_copy;
}

static ssize_t hello_write(struct file *file, const char __user *user_buf,
                            size_t count, loff_t *offset)
{
    int ret;
    size_t bytes_to_copy;

    if (count > BUF_SIZE) {
        bytes_to_copy = BUF_SIZE;
        pr_info("%s: WARNING: write() called, count=%zu, bytes_to_copy=%zu, dropped_bytes=%zu\n", DEVICE_NAME, count, bytes_to_copy, count - bytes_to_copy);

    } else {
        bytes_to_copy = count;
        pr_info("%s: write() called, count=%zu, bytes_to_copy=%zu\n", DEVICE_NAME, count, bytes_to_copy);
    }


    ret = copy_from_user(device_buf, user_buf, bytes_to_copy);
    if (ret != 0) return -EFAULT;

    buf_len = bytes_to_copy;

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

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
    .write = hello_write,
};

static int __init hello_init(void) {
    pr_info("%s: intitiazing...\n", DEVICE_NAME);
    int ret;
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret != 0) {
        pr_info("%s: alloc_chrdev_region error_code: %d...\n", DEVICE_NAME, ret);
        return ret;
    }

    cdev_init(&my_cdev, &fops);

    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret != 0) {
        pr_info("%s: cdev_add error_code: %d...\n", DEVICE_NAME, ret);
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    pr_info("%s: module loaded successfully.\n", DEVICE_NAME);
    return 0;
}


static void __exit hello_exit(void) {
    pr_info("%s: exiting...\n", DEVICE_NAME);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("%s: module unloaded successfully.\n", DEVICE_NAME);
    return;
}


module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("manual char device registration");
