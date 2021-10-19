#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/fs.h>

#define mem_size	1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mpivet-p");
MODULE_DESCRIPTION("Little-penguin's misc char device driver");
MODULE_VERSION("1.0");

struct dentry *init_id(struct dentry *dbg_root);
struct dentry *init_jiffies(struct dentry *dbg_root);
struct dentry *init_foo(struct dentry *dbg_root);

struct file_operations dbg_fops = {
     .owner = THIS_MODULE,
     //.open = dgb_open,
     //.read = dbg_read,
     //.write = dbg_write,
};

struct dentry *debugfs_root;
struct dentry *id;
struct dentry *jiffs;
struct dentry *foo;

static int __init init_driver(void)
{
	debugfs_root = debugfs_create_dir("fortytwo", NULL);
	id = init_id(debugfs_root);
	jiffs = init_jiffies(debugfs_root);
	foo = init_foo(debugfs_root);
	return (0);
}

static void __exit exit_driver(void)
{
	debugfs_remove(foo);
	debugfs_remove(jiffs);
	debugfs_remove(id);
	debugfs_remove(debugfs_root);
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(init_driver);
module_exit(exit_driver);
