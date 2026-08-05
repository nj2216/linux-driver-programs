#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/of.h> /* Required for Device Tree APIs */

#define DRIVER_NAME "myramdisk"

struct myramdisk_dev {
    char *buffer;
    size_t size;
    struct miscdevice misc;
};

/* --- File Operations --- */

static ssize_t myramdisk_read(struct file *file, char __user *user_buf, size_t count, loff_t *offset)
{
    struct myramdisk_dev *dev = container_of(file->private_data, struct myramdisk_dev, misc);
    unsigned long uncopied;
    size_t bytes_copied;

    if (*offset >= dev->size)
        return 0;

    if (*offset + count > dev->size)
        count = dev->size - *offset;

    uncopied = copy_to_user(user_buf, dev->buffer + *offset, count);
    bytes_copied = count - uncopied;

    if (bytes_copied == 0)
        return -EFAULT;

    *offset += bytes_copied;
    return bytes_copied;
}

static ssize_t myramdisk_write(struct file *file, const char __user *user_buf, size_t count, loff_t *offset)
{
    struct myramdisk_dev *dev = container_of(file->private_data, struct myramdisk_dev, misc);
    unsigned long uncopied;
    size_t bytes_copied;

    if (*offset >= dev->size)
        return -ENOSPC;

    if (*offset + count > dev->size)
        count = dev->size - *offset;

    uncopied = copy_from_user(dev->buffer + *offset, user_buf, count);
    bytes_copied = count - uncopied;

    if (bytes_copied == 0)
        return -EFAULT;

    *offset += bytes_copied;
    return bytes_copied;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = myramdisk_read,
    .write = myramdisk_write,
};

/* --- Probe & Remove --- */

static int myramdisk_probe(struct platform_device *pdev)
{
    struct myramdisk_dev *dev;
    u32 ram_size = 4096; /* Default fallback */
    int ret;

    pr_info("myramdisk: PROBE called for DT node '%s'\n", pdev->dev.of_node->full_name);

    /* Read 'ram-size' property from the Device Tree Node! */
    ret = of_property_read_u32(pdev->dev.of_node, "ram-size", &ram_size);
    if (ret == 0) {
        pr_info("myramdisk: Successfully extracted 'ram-size' = %u bytes from Device Tree!\n", ram_size);
    } else {
        pr_warn("myramdisk: Could not read 'ram-size' from DT, falling back to %u\n", ram_size);
    }

    /* Allocate device state */
    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->size = ram_size;

    /* Allocate buffer matching DT requested size */
    dev->buffer = devm_kzalloc(&pdev->dev, dev->size, GFP_KERNEL);
    if (!dev->buffer)
        return -ENOMEM;

    platform_set_drvdata(pdev, dev);

    /* Register device node */
    dev->misc.minor = MISC_DYNAMIC_MINOR;
    dev->misc.name  = DRIVER_NAME;
    dev->misc.fops  = &fops;

    ret = misc_register(&dev->misc);
    if (ret) {
        pr_err("myramdisk: failed to register misc device\n");
        return ret;
    }

    pr_info("myramdisk: Device Tree Driver initialized with %zu bytes at /dev/%s\n", dev->size, DRIVER_NAME);
    return 0;
}

static void myramdisk_remove(struct platform_device *pdev)
{
    struct myramdisk_dev *dev = platform_get_drvdata(pdev);

    misc_deregister(&dev->misc);
    pr_info("myramdisk: DT device removed\n");
    return;
}

/* Device Tree Compatible Table */
static const struct of_device_id myramdisk_of_match[] = {
    { .compatible = "jeevan,myramdisk", },
    { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, myramdisk_of_match);

static struct platform_driver myramdisk_driver = {
    .probe  = myramdisk_probe,
    .remove = myramdisk_remove,
    .driver = {
        .name           = DRIVER_NAME,
        .of_match_table = myramdisk_of_match,
    },
};

module_platform_driver(myramdisk_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("Device Tree Platform Driver");