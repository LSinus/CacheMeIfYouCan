obj-m += allocdev.o
obj-m += rdd.o

KERNEL_VERSION=linux-5.14

# Use an absolute path if possible, but relative is okay if the structure is rigid
KDIR := ../../$(KERNEL_VERSION)
# Force the current directory to be evaluated correctly
MY_PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(MY_PWD) modules
	cp allocdev.ko ../initramfs
	cp rdd.ko ../initramfs
	cp rdd* ../../$(KERNEL_VERSION)
	cp allocdev* ../../$(KERNEL_VERSION)

clean:
	$(MAKE) -C $(KDIR) M=$(MY_PWD) clean
