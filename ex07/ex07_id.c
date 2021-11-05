#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define mem_size	10

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
	if (copy_to_user(buf, "mpivet-p\n", 10)) {
		return (0);
	}
	return (9);
}

static ssize_t  mcdd_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
	if (len > mem_size || copy_from_user(kernel_buffer, buf, len)) {
		return (-EINVAL);
	}
	if ((strlen(kernel_buffer) == 8 && strcmp(kernel_buffer, "mpivet-p") == 0)
		|| (strlen(kernel_buffer) == 9 && strcmp(kernel_buffer, "mpivet-p\n") == 0)) {
		return (len);
	}
	return (-EINVAL);
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
	.write		= mcdd_write,
	.open		= mcdd_open,
	.release	= mcdd_release,
};

struct dentry *init_id(struct dentry *dbg_root)
{
	return (debugfs_create_file("id", 0666, dbg_root, NULL, &fops));
}
