obj-m += allocdev.o
obj-m += rdd.o

# Use an absolute path if possible, but relative is okay if the structure is rigid
KDIR := ../../linux-6.2
# Force the current directory to be evaluated correctly
MY_PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(MY_PWD) modules
	cp allocdev.ko ../initramfs
	cp rdd.ko ../initramfs

clean:
	$(MAKE) -C $(KDIR) M=$(MY_PWD) clean
