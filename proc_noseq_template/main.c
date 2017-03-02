#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>

#define LOG_FUNC_B() printk(KERN_INFO "%s +\n", __func__)
#define LOG_FUNC_E() printk(KERN_INFO "%s +\n", __func__)

#define PROC_DIR_NAME "xxx_dir"
#define PROC_ENTRY_NAME "xxx_1"

#define PROC_MAX_SIZE 12
static char str_buf[PROC_MAX_SIZE] = "HelloWorld!";

struct proc_dir_entry *dir;
struct proc_dir_entry *entry;

ssize_t xxx_proc_read(struct file *filp, char __user *buf, size_t count,
		      loff_t *f_pos)
{
    int ret=0;
    if(*f_pos == 0) {
        printk(KERN_INFO "procfile read %s\n", filp->f_path.dentry->d_name.name);
        copy_to_user(buf, str_buf, PROC_MAX_SIZE);
        ret=PROC_MAX_SIZE;
	*f_pos += ret;
    }
    return ret;

}

ssize_t xxx_proc_write(struct file *filp, const char __user *buf, size_t count,
		  loff_t *f_pos) {
	int write_cnt = 0;

	if (count > PROC_MAX_SIZE - *f_pos)
		return -EINVAL;

	write_cnt = min(count, PROC_MAX_SIZE - *f_pos);

	copy_from_user(str_buf + *f_pos, buf, write_cnt);

	*f_pos += write_cnt;

	return write_cnt;
}

static struct file_operations xxx_proc_fops = {
	.owner = THIS_MODULE,
	.read = xxx_proc_read,
	.write = xxx_proc_write,
};

static int __init xxx_module_init(void)
{
	LOG_FUNC_B();

	dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!dir) {
		proc_remove(dir);
		printk(KERN_ERR "create proc dir error\n");
		return -ENOMEM;
	}

	entry = proc_create(PROC_ENTRY_NAME, 0666, dir, &xxx_proc_fops);
	if (!entry) {
		proc_remove(entry);
		proc_remove(dir);
		printk(KERN_ERR "create proc entry error\n");
		return -ENOMEM;
	}

	LOG_FUNC_E();
	return 0;
}

static void __exit xxx_module_exit(void)
{
	LOG_FUNC_B();

	proc_remove(entry);
	proc_remove(dir);

	LOG_FUNC_E();
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elinx Hsi");

module_init(xxx_module_init);
module_exit(xxx_module_exit);
