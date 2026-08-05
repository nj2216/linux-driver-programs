#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "myramdisk"
#define RAMDISK_SIZE 4096

struct myramdisk_dev {
    char* buffer;
    size_t size;
    struct miscdevice misc;
};

static struct myramdisk_dev *ramdisk_dev;

static ssize_t myramdisk_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset) {
    size_t bytes_copied;
    size_t uncopied;

    if (*offset >= RAMDISK_SIZE) {
        return 0;
    }

    if (*offset + count > RAMDISK_SIZE) {
        count = RAMDISK_SIZE - *offset;
    }

    uncopied = copy_to_user(user_buf, (ramdisk_dev->buffer + *offset), 0);
    bytes_copied = count - uncopied;

    if (bytes_copied == 0) {
        return -EFAULT;
    }

    *offset += bytes_copied;
    return bytes_copied;
}

static ssize_t myramdisk_write(struct file *file, const char __user *user_buf, size_t count, loff_t *offset) {
    size_t bytes_copied;
    size_t uncopied;
    
    if (*offset >= RAMDISK_SIZE) {
        return -ENOSPC;
    }
    
    if (*offset + count > RAMDISK_SIZE) {
        count = RAMDISK_SIZE - *offset;
    }
    
    uncopied = copy_from_user((ramdisk_dev-> buffer + *offset), user_buf, 0);
    bytes_copied = count - uncopied;

    if (bytes_copied == 0) {
        return -EFAULT;
    }

    *offset += bytes_copied;
    return bytes_copied;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = myramdisk_read,
    .write = myramdisk_write,
};

static int __init myramdisk_init(void) {
    int ret;

    // 1. Allocate device structure
    ramdisk_dev = kzalloc(sizeof(*ramdisk_dev), GFP_KERNEL);
    if (!ramdisk_dev) {
        return -ENOMEM;
    }

    ramdisk_dev->size = RAMDISK_SIZE;

    // 2. Allocate RAM disk buffer
    ramdisk_dev->buffer = kzalloc(RAMDISK_SIZE, GFP_KERNEL);
    if (!ramdisk_dev->buffer) {
        ret = -ENOMEM;
        goto err_free_dev;
    }

    // 3. Register misc device
    ramdisk_dev->misc.minor = MISC_DYNAMIC_MINOR;
    ramdisk_dev->misc.name = DEVICE_NAME;
    ramdisk_dev->misc.fops = &fops;

    ret = misc_register(&ramdisk_dev->misc);
    if (ret) {
        pr_err("myramdisk: failed to register device\n");
        goto err_free_buf;
    }

    pr_info("myramdisk: allocated %d bytes RAM disk at /dev/%s\n", RAMDISK_SIZE, DEVICE_NAME);
    return 0;

err_free_buf:
    kfree(ramdisk_dev->buffer);
err_free_dev:
    kfree(ramdisk_dev);
    return ret;    
}

static void __exit myramdisk_exit(void) {
    misc_deregister(&ramdisk_dev->misc);
    kfree(ramdisk_dev->buffer);
    kfree(ramdisk_dev);
    pr_info("myramdisk: unregistered and memory freed\n");
}

module_init(myramdisk_init);
module_exit(myramdisk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("RAM disk character driver");