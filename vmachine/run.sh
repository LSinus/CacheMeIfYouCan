#!/bin/zsh

# Stop the script immediately if any command returns an error
set -e

echo "[+] Compiling exploit..."
make

echo "[+] Copying to initramfs..."
cp ./exploit ./initramfs/exploit

echo "[+] Repacking initramfs..."
../scripts/pack_initramfs.sh

echo "[+] Mounting disk..."
DISK="./disk.raw"
MOUNT_POINT="/mnt/vmachinedisk"
LOOP=$(sudo losetup -fP --show $DISK)
echo "    Loop device: $LOOP"
sudo mount $LOOP $MOUNT_POINT

mkdir -p ./initramfs/home
mkdir -p ./initramfs/home/user

sudo cp -R ./initramfs/* $MOUNT_POINT

sudo mkdir -p $MOUNT_POINT/proc
sudo mkdir -p $MOUNT_POINT/sys
sudo mkdir -p $MOUNT_POINT/dev
sudo mkdir -p $MOUNT_POINT/dev/pts
sudo mkdir -p $MOUNT_POINT/usr/bin

sudo chown -R 1000:1000 $MOUNT_POINT/home/user
sudo chmod 640 $MOUNT_POINT/etc/shadow
sudo chmod 644 $MOUNT_POINT/etc/passwd

sudo umount $MOUNT_POINT
sudo losetup -d $LOOP
echo "    Done, loop device released"


echo "[+] Booting QEMU..."

    #    -initrd ./initramfs.cpio.gz \
taskset -c 1 qemu-system-x86_64 \
    -kernel ./../linux-6.2/arch/x86/boot/bzImage \
    -drive file=disk.raw,format=raw,if=virtio \
    -m 10G \
    -enable-kvm \
    -cpu host,pcid=off \
    -smp 1 \
    -append "root=/dev/vda rw init=/init console=ttyS0 oops=panic panic=1 kpti=1 nokaslr quiet hugepagesz=1G hugepages=5" \
    -serial mon:stdio \
    -nographic \
    -s \
    -device virtio-serial \
    -chardev file,id=char0,path=dump.bin \
    -device virtserialport,chardev=char0,name=exploit_data \
    -netdev user,id=net0,hostfwd=tcp::5555-:23 \
    -device e1000,netdev=net0 \
