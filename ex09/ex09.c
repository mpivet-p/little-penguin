#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/nsproxy.h>
#include <linux/types.h>
#include <linux/ns_common.h>
#include <linux/string.h>
#include "mount.h"

#define mem_size	1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mpivet-p");
MODULE_DESCRIPTION("Little-penguin's assignment 09. Create a module that\
		lists all mountpoints");
MODULE_VERSION("1.0");

char mounts[PAGE_SIZE];
static int ok = 1;

static ssize_t read_proc(struct file *filp, char __user *buffer
		, size_t length, loff_t * offset)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;
	char path_buf[256];
	char buf[512];
	char *path;

	if (ok) {
		ok = 0;
	} else {
		ok = 1;
		return (0);
	}
	memset(mounts, 0, PAGE_SIZE);
	list_for_each_entry(mnt, &(ns->list), mnt_list) {
		if (!mnt->mnt_mp)
			continue ;
		memset(path_buf, 0, 256);
		path = dentry_path_raw(mnt->mnt_mountpoint, path_buf, 256);
		sprintf(buf, "%-20s %s\n", mnt->mnt_devname, path);
		strcat(mounts, buf);
	}
	copy_to_user(buffer, mounts, strlen(mounts));
	return length;
}

static struct file_operations proc_fops =
{
	.read	= read_proc,
};

static int __init init_driver(void)
{
	proc_create("mymounts", 0666, NULL, &proc_fops);
	return (0);
}

static void __exit exit_driver(void)
{
	remove_proc_entry("mymounts", NULL);
}

module_init(init_driver);
module_exit(exit_driver);
