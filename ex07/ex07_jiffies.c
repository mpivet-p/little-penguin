#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#define mem_size	1024

static uint8_t *kernel_buffer;
static int print_protection = 1;

static ssize_t  mcdd_read(struct file *filp, char __user *buf, size_t len, loff_t * off)
{
	if (print_protection) {
		print_protection = 0;
	} else {
		print_protection = 1;
		return (0);
	}
	sprintf(kernel_buffer, "%lu\n", jiffies);
	if (copy_to_user(buf, kernel_buffer, strlen(kernel_buffer))) {
		return (0);
	}
	return (strlen(kernel_buffer));
}

static int mcdd_release(struct inode *inode, struct file *file)
{
        kfree(kernel_buffer);
        return 0;
}

static int mcdd_open(struct inode *inode, struct file *file)
{
	if ((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0) {
		pr_info("Cannot allocate memory in kernel\n");
		return -1;
	}
	return 0;
}

static struct file_operations fops =
{
	.owner		= THIS_MODULE,
	.read		= mcdd_read,
	.open		= mcdd_open,
	.release	= mcdd_release,
};

struct dentry *init_jiffies(struct dentry *dbg_root)
{
	return (debugfs_create_file("jiffies", 0444, dbg_root, NULL, &fops));
}
