// SPDX-License-Identifier: GPL v2
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user,
		size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user,
		size_t size, loff_t *offs);

static struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};
static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE + 1];

static int __init myfd_init(void)
{
	int retval;

	retval = misc_register(&myfd_device);
	if (retval != 0)
		pr_info("Failed to register `reverse' device\n");
	return retval;
}

static void __exit myfd_cleanup(void)
{

}

ssize_t myfd_read(struct file *fp, char __user *user
		, size_t size, loff_t *offs)
{
	size_t len;

	len = strlen(str);
	return simple_read_from_buffer(user, size, offs, str, len);
}

ssize_t myfd_write(struct file *fp, const char __user *user
		, size_t size, loff_t *offs)
{
	ssize_t res;

	res = simple_write_to_buffer(str, PAGE_SIZE, offs, user, PAGE_SIZE) + 1;
	/*
	 * 0x0 = '\0'
	 */
	str[res] = 0x0;
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
