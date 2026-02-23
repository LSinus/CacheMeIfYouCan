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
