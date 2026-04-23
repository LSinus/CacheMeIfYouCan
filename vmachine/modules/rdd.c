#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h> 
#include <linux/ioctl.h>

#define DEVICE_NAME "rdd"
#define CLASS_NAME  "rdd_class"

MODULE_LICENSE("GPL");

static int major;
static struct class *rdd_class = NULL;
static struct device *rdd_device = NULL;

typedef struct msg_t {
    size_t uaddr, size;
} msg_t;

#define RDD_MAGIC 'k'
#define RDD_ALLOC _IOW(RDD_MAGIC, 1, msg_t)
#define RDD_FREE  _IOW(RDD_MAGIC, 2, msg_t)
#define RDD_READ  _IOWR(RDD_MAGIC, 3, msg_t)
#define RDD_BP    _IOW(RDD_MAGIC, 4, msg_t)

static noinline void rdd_bp(void) {
	//printk("Kernel breakpoint on rdd module\n");
	asm volatile ("");
}

static volatile void *obj;

static long int rdd_ioctl(struct file *file, unsigned int num, long unsigned data) {
    msg_t msg;
    unsigned long ret = copy_from_user(&msg, (void *)data, sizeof(msg_t));
    //if(ret) return -1;

    switch(num) {
    case RDD_ALLOC:
        obj = kmalloc(msg.size, GFP_KERNEL); 
        pr_info("rdd: obj allocated at %px\n", obj);
        return 0;
    case RDD_FREE:
        kfree((void *)obj); return 0;
    case RDD_READ:
        ret = copy_to_user((void *)msg.uaddr, (void *)obj, msg.size);
        if(ret) return -1;
        return 0;
	case RDD_BP:
		rdd_bp();
		return 0;
    default:
        return -1;
    }
}

static struct file_operations fops = {
    .owner = THIS_MODULE, 
    .unlocked_ioctl = rdd_ioctl,
};

static int __init rdd_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    rdd_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(rdd_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(rdd_class);
    }

    rdd_device = device_create(rdd_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(rdd_device)) {
        class_destroy(rdd_class); 
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(rdd_device);
    }

    pr_info("rdd: Device created at /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit rdd_exit(void) {
    device_destroy(rdd_class, MKDEV(major, 0));
    class_destroy(rdd_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("rdd: Unloaded\n");
}

module_init(rdd_init);
module_exit(rdd_exit)
