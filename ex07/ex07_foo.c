#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define mem_size	1024

//uint8_t *kernel_buffer;
static int print_protection = 1;
/*
static ssize_t  mcdd_read(struct file *filp, char __user *buf, size_t len, loff_t * off)
{
	if (print_protection) {
		print_protection = 0;
	} else {
		print_protection = 1;
		return (0);
	}
	strcpy(kernel_buffer, "mpivet-p\n");
	if (copy_to_user(buf, kernel_buffer, mem_size)) {
		return (0);
	}
	return (9);
}
static ssize_t  mcdd_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
	if (copy_from_user(kernel_buffer, buf, len)) {
		return (0);
	}
	if (strcmp(kernel_buffer, "mpivet-p") == 0) {
		return (len);
	}
	return (0);
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
*/

static struct file_operations fops =
{
	.owner		= THIS_MODULE,
	//.read		= mcdd_read,
	//.write		= mcdd_write,
	//.open		= mcdd_open,
	//.release	= mcdd_release,
};

struct dentry *init_foo(struct dentry *dbg_root)
{
	return (debugfs_create_file("foo", 0644, dbg_root, NULL, &fops));
}