// SPDX-License-Identifier: GPL v2
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user,
			 size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user,
			  size_t size, loff_t *offs);

static const struct file_operations myfd_fops = {
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
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user
		, size_t size, loff_t *offs)
{
	return simple_read_from_buffer(user, size, offs, str, strlen(str));
}

ssize_t myfd_write(struct file *fp, const char __user *user
		, size_t size, loff_t *offs)
{
	char *tmp;
	ssize_t res;
	size_t i = 0;
	ssize_t t;

	tmp = kmalloc(sizeof(char) * PAGE_SIZE + 1, GFP_KERNEL);
	if (!tmp)
		return (-ENOMEM);
	res = simple_write_to_buffer(tmp, PAGE_SIZE, offs, user, size);
	if (res < 0)
		return (res);
	res--;
	tmp[res] = 0;
	for (t = res - 1; t >= 0; t--, i++)
		str[i] = tmp[t];
	str[i] = '\n';
	str[i + 1] = 0;
	return size;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
