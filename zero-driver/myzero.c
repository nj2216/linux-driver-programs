#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "myzero"

static ssize_t myzero_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset) {
    unsigned long uncleared;
    size_t written;

    if (count == 0) {
        return 0;
    }

    uncleared = clear_user(user_buf, count);

    written = count - uncleared;

    if(written == 0) {
        return -EFAULT;
    }

    *offset += written;

    return written;
}

static ssize_t myzero_write(struct file *file, const char __user *user_buf, size_t count, loff_t *offset) {
    return count;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = myzero_read,
    .write = myzero_write,
};

static struct miscdevice myzero_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DRIVER_NAME,
    .fops = &fops,
};

static int __init myzero_init(void) {
    int ret;
    
    ret = misc_register(&myzero_misc);
    if (ret) {
        pr_err("myzero: failed to register misc device (err %d)\n", ret);
        return ret;
    }

    pr_info("myzero: registered successfully as /dev/%s\n", DRIVER_NAME);
    return 0;
}

static void __exit myzero_exit(void) {
    misc_deregister(&myzero_misc);
    pr_info("myzero: unregistered successfully\n");
}

module_init(myzero_init);
module_exit(myzero_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("A simple zero-like misc device driver");