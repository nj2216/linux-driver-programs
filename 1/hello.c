#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

static int count = 1;
module_param(count, int, 0644);
MODULE_PARM_DESC(count, "A simple integer counter.");

static int __init hello_init(void) {
    int i;
    for (i= 0; i < count; i++) {
        pr_info("hello: count %d\n", i);
    }
    return 0;
}

static void __exit hello_exit(void) {
    pr_info("hello: module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeevan");
MODULE_DESCRIPTION("hello world kernel module");
