#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mynull"

static ssize_t mynull_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset) {
    return 0;
}

static ssize_t mynull_write(struct file *file, const char __user *user_buf, size_t count, loff_t *offset) {
    return count;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = mynull_read,
    .write = mynull_write,
};

static struct miscdevice mynull_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &fops,
};

static int __init mynull_init(void) {
    int ret;

    ret = misc_register(&mynull_misc);
    if (ret) {
        pr_err("mynull: failed to register misc device (err %d)\n", ret);
        return ret;
    }
    
    pr_info("mynull: registered successfully as /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit mynull_exit(void) {
    misc_deregister(&mynull_misc);
    pr_info("mynull: unregistered\n");
}

module_init(mynull_init);
module_exit(mynull_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("A simple /dev/null clone driver");