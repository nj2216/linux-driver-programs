#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define DRIVER_NAME "hello_platform"

static int hello_probe(struct platform_device *pdev) {
    pr_info("%s: probe() was called by %d:%s\n", DRIVER_NAME, pdev->id, pdev->name);
    return 0;
}

static void hello_remove(struct platform_device *pdev) {
    pr_info("%s: remove() was called by %d:%s\n", DRIVER_NAME, pdev->id, pdev->name);
}

static struct platform_driver hello_platform_driver = {
    .probe = hello_probe,
    .remove = hello_remove,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
};

static struct platform_device *hello_pdev;

static int __init hello_init(void) {
    pr_info("%s: initialising..\n", DRIVER_NAME);
    int ret;
    ret = platform_driver_register(&hello_platform_driver);
    if (ret != 0) {
        return ret;
    }

    hello_pdev = platform_device_register_simple(DRIVER_NAME, -1, NULL, 0);
    if (IS_ERR(hello_pdev)) {
        ret = PTR_ERR(hello_pdev);
        pr_info("%s: platform_device_register_simple failed: %d\n", DRIVER_NAME, ret);
        platform_driver_unregister(&hello_platform_driver);
        return ret;
    }
    pr_info("%s: module loaded and driver registered\n", DRIVER_NAME);
    return 0;
}

static void __exit hello_exit(void) {
    pr_info("%s: device unregistering...\n", DRIVER_NAME);
    platform_device_unregister(hello_pdev);
    platform_driver_unregister(&hello_platform_driver);
    pr_info("%s: devices unregistered and module unloaded successfully\n", DRIVER_NAME);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("platform driver skeleton");