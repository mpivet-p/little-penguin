#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>

#define mem_size	32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mpivet-p");
MODULE_DESCRIPTION("Little-penguin's misc char device driver");
MODULE_VERSION("1.0");

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
uint8_t *kernel_buffer;
static int print_protection = 1;

static ssize_t  mcdd_read(struct file *filp, char __user *buf, size_t len, loff_t * off)
{
	if (print_protection) {
		print_protection = 0;
	} else {
		print_protection = 1;
		return (0);
	}
	if (copy_to_user(buf, "mpivet-p\n", 9)) {
		return (0);
	}
	return (9);
}

static ssize_t  mcdd_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
	if (len > mem_size || copy_from_user(kernel_buffer, buf, len)) {
		return (-EINVAL);
	}
	if ((strlen(kernel_buffer) == 8
				&& strncmp(kernel_buffer, "mpivet-p", 8) == 0)
			|| (strlen(kernel_buffer) == 9
				&& strncmp(kernel_buffer, "mpivet-p\n", 9) == 0)) {
		return (len);
	}
	return (-EINVAL);
}

static int mcdd_release(struct inode *inode, struct file *file)
{
        kfree(kernel_buffer);
        return (0);
}

static int mcdd_open(struct inode *inode, struct file *file)
{
	if ((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0) {
		pr_info("Cannot allocate memory in kernel\n");
		return (ENOMEM);
	}
	memset(kernel_buffer, 0, mem_size);
	return (0);
}

static struct file_operations fops =
{
	.owner		= THIS_MODULE,
	.read		= mcdd_read,
	.write		= mcdd_write,
	.open		= mcdd_open,
	.release	= mcdd_release,
};

static int __init init_driver(void)
{
	if(alloc_chrdev_region(&dev, 0, 1, "fortytwo") < 0) {
		printk(KERN_NOTICE "Cannot allocate major number for device 1\n");
		return -1;
	}
	cdev_init(&etx_cdev, &fops);
	if (cdev_add(&etx_cdev, dev, 1) >= 0) {
		if ((dev_class = class_create(THIS_MODULE,"ft_class")) != NULL) {
			if ((device_create(dev_class, NULL, dev, NULL, "fortytwo")) != NULL) {
				pr_info("Kernel Module Inserted Successfully...\n");
				return (0);
			} else {
				pr_err("Cannot create the Device\n");
				class_destroy(dev_class);
				device_destroy(dev_class,dev);
			}
		} else {
			class_destroy(dev_class);
		}
	} else {
		pr_err("Cannot add the device to the system\n");
	}
	unregister_chrdev_region(dev,1);
	return (-1);
}

static void __exit exit_driver(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	unregister_chrdev_region(dev, 1);
}

module_init(init_driver);
module_exit(exit_driver);
