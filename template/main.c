#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/device.h>
#include <linux/sched.h>

#define LOG_FUNC_B() printk(KERN_INFO "%s +\n", __func__)
#define LOG_FUNC_E() printk(KERN_INFO "%s +\n", __func__)

static int __init xxx_module_init(void)
{
	LOG_FUNC_B();
	return 0;
}

static void __exit xxx_module_exit(void)
{
	LOG_FUNC_E();
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elinx Hsi");

module_init(xxx_module_init);
module_exit(xxx_module_exit);
