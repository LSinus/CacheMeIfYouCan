#pragma GCC optimize ("O0")
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h> 

#define DEVICE_NAME "allocdev"
#define CLASS_NAME  "alloc_class"

MODULE_LICENSE("GPL");

#define BUFSIZE 64
#define SUCCESS 0

static char *alloc_buf;
static int major;
static struct class *alloc_class = NULL;
static struct device *alloc_device = NULL;

static int allocdev_open(struct inode *inode, struct file *file) {
   alloc_buf = kmalloc(BUFSIZE, GFP_KERNEL);

   if(!alloc_buf) return -EINVAL;

   file->private_data = alloc_buf;

   return SUCCESS;
}

static int allocdev_close(struct inode *inode, struct file *file) {
    if(!file->private_data) return -EINVAL; 
    kfree(file->private_data);
    return SUCCESS;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE, 
    .open = allocdev_open,
    .release = allocdev_close
};

static int __init allocdev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    alloc_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(alloc_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(alloc_class);
    }

    alloc_device = device_create(alloc_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(alloc_device)) {
        class_destroy(alloc_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(alloc_device);
    }

    pr_info("allocdev: Device created at /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit allocdev_exit(void) {
    device_destroy(alloc_class, MKDEV(major, 0));
    class_destroy(alloc_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("allocdev: Unloaded\n");
}

module_init(allocdev_init);
module_exit(allocdev_exit);
