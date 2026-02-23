#!/bin/zsh

# Stop the script immediately if any command returns an error
set -e

echo "[+] Compiling exploit..."
make

echo "[+] Copying to initramfs..."
cp ./exploit ./initramfs/exploit

echo "[+] Repacking initramfs..."
../scripts/pack_initramfs.sh

echo "[+] Booting QEMU..."

taskset -c 1 qemu-system-x86_64 \
    -kernel ./../linux-6.2/arch/x86/boot/bzImage \
    -initrd ./initramfs.cpio.gz \
    -m 10G \
    -enable-kvm \
    -cpu host \
    -smp 1 \
    -append "console=ttyS0 oops=panic panic=1 nokaslr quiet hugepagesz=1G hugepages=5" \
    -serial mon:stdio \
    -nographic \
    -s \
    -device virtio-serial \
    -chardev file,id=char0,path=dump.bin \
    -device virtserialport,chardev=char0,name=exploit_data
