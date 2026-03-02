#!/bin/bash

KERNEL_VERSION="6.2"
URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-$KERNEL_VERSION.tar.xz"

echo "Downloading Linux $KERNEL_VERSION tarball to the main folder..."

curl -L $URL -o "linux-$KERNEL_VERSION.tar.xz"

if [ $? -eq 0 ]; then
    echo "Download complete: linux-$KERNEL_VERSION.tar.xz"
else
    echo "Download failed. Please check your internet connection."
    exit 1
fi

echo "Setting up disk image for virtual machine..."
DISK="disk.raw"
MOUNT_POINT="/mnt/vmachinedisk"

qemu-img create -f raw vmachine/$DISK 2G
LOOP=$(sudo losetup -fP --show vmachine/disk.raw)
sudo mount $LOOP $MOUNT_POINT
sudo mkfs.ext4 $LOOP
sudo umount $MOUNT_POINT
sudo losetup -d $LOOP

echo "Done"
