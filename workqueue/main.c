#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/init.h>


#define LOG_FUNC_B() printk(KERN_INFO "%s +\n", __func__)
#define LOG_FUNC_E() printk(KERN_INFO "%s +\n", __func__)

#define PROC_DIR_NAME "xxx_dir"
#define PROC_ENTRY_NAME "xxx_1"

#define PROC_MAX_SIZE 12
static char str_buf[PROC_MAX_SIZE] = "HelloWorld!";

static struct proc_dir_entry *dir;
static struct proc_dir_entry *entry;

#define XXX_WQ_NAME "xxx_wq"
static struct workqueue_struct *xxx_wq;

static struct delayed_work xxx_task;
static void xxx_task_handler(struct work_struct *data);
static DECLARE_DELAYED_WORK(xxx_task, xxx_task_handler);

static int died = 0;

static void xxx_task_handler(struct work_struct *data)
{
	printk(KERN_INFO "%s running...\n", __func__);
	if (!died)
		queue_delayed_work(xxx_wq, &xxx_task, 100);
}

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

int xxx_proc_open(struct inode *inode, struct file *filp)
{
	try_module_get(THIS_MODULE);
	return 0;
}

int xxx_proc_close(struct inode *inode, struct file *filp)
{
	module_put(THIS_MODULE);
	return 0;
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

	proc_set_size(entry, 20);
	proc_set_user(entry, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);

	xxx_wq = create_workqueue(XXX_WQ_NAME);
	if (!xxx_wq) {
		printk(KERN_ERR "create work queue error\n");
		return -1;
	}
	queue_delayed_work(xxx_wq, &xxx_task, 100);

	LOG_FUNC_E();
	return 0;
}

static void __exit xxx_module_exit(void)
{
	LOG_FUNC_B();

	proc_remove(entry);
	proc_remove(dir);

	died = 1;
	cancel_delayed_work(&xxx_task);
	flush_workqueue(xxx_wq);
	destroy_workqueue(xxx_wq);

	LOG_FUNC_E();
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elinx Hsi");

module_init(xxx_module_init);
module_exit(xxx_module_exit);
