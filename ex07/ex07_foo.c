#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define mem_size	1024

//uint8_t *kernel_buffer;
static char buffer[PAGE_SIZE + 1];
static int print_protection = 1;
static int locked = 0;

static ssize_t  foo_read(struct file *filp, char __user *buf, size_t len, loff_t * off)
{
	if (locked != 0)
		return (0);
	if (print_protection) {
		print_protection = 0;
	} else {
		print_protection = 1;
		return (0);
	}
	locked = 1;
	if (copy_to_user(buf, buffer, strlen(buffer))) {
		locked = 0;
		return (0);
	}
	locked = 0;
	return (9);
}

static ssize_t  foo_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
	if (locked != 0)
		return (0);
	locked = 1;
	memset(buffer, 0, PAGE_SIZE + 1);
	if (len > PAGE_SIZE || copy_from_user(buffer, buf, len)) {
		locked = 0;
		return (0);
	}
	locked = 0;
	return (len);
}

static struct file_operations fops =
{
	.owner		= THIS_MODULE,
	.read		= foo_read,
	.write		= foo_write,
};

struct dentry *init_foo(struct dentry *dbg_root)
{
	return (debugfs_create_file("foo", 0644, dbg_root, NULL, &fops));
}
