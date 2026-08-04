#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#define DRIVER_NAME "hello_platform"

static int hello_probe(struct platform_device *pdev) {
    int ret;
    u32 greeting_count;
    pr_info("%s: probe() was called by %d:%s\n", DRIVER_NAME, pdev->id, pdev->name);

    ret = of_property_read_u32(pdev->dev.of_node, "jeevan,greeting-count", &greeting_count);
    if (ret != 0) {
        greeting_count = 1; // sensible default when property is missing
        pr_info("%s: WARNING: greeting_count unreadable (err=%d), defaulting to %u\n", DRIVER_NAME, ret, greeting_count);    
    } else {
        pr_info("%s: Greeting_count: %u\n", DRIVER_NAME, greeting_count);
    }

    return 0;
}

static void hello_remove(struct platform_device *pdev) {
    pr_info("%s: remove() was called by %d:%s\n", DRIVER_NAME, pdev->id, pdev->name);
}

static const struct of_device_id hello_of_match[] = {
    {.compatible = "jeevan,hello-platform"},
    {},
};

MODULE_DEVICE_TABLE(of, hello_of_match);

static struct platform_driver hello_platform_driver = {
    .probe = hello_probe,
    .remove = hello_remove,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = hello_of_match,
    },
};

static int __init hello_init(void) {
    pr_info("%s: initialising..\n", DRIVER_NAME);
    int ret;
    ret = platform_driver_register(&hello_platform_driver);
    if (ret != 0) {
        return ret;
    }
    pr_info("%s: module loaded and driver registered\n", DRIVER_NAME);
    return 0;
}

static void __exit hello_exit(void) {
    platform_driver_unregister(&hello_platform_driver);
    pr_info("%s: module unloaded successfully\n", DRIVER_NAME);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan N");
MODULE_DESCRIPTION("platform driver skeleton");