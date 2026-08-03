#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEVICE_NAME "hellochar"

static dev_t dev_num;
static struct cdev my_cdev;

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
