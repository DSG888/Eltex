#!/usr/bin/env bash
mkdir /tmp/rootfs
mkdir /tmp/src
tar -xf ./linux.tar.xz -C /tmp/src
tar -xf ./busybox.tar.bz2 -C /tmp/src

cd /tmp/src/busybox-1.30.1
export "ARCH=arm"
export "CROSS_COMPILE=arm-linux-gnueabihf-"
export "CONFIG_STATIC=y"
make defconfig
make -j9
make CONFIG_PREFIX=/tmp/rootfs install

cd /tmp/rootfs
echo -e \#"${1//$/\\$!}!""/bin/sh\n/bin/mount -t devtmpfs devtmpfs /dev\n/bin/mount -t proc none /proc\n/bin/mount -t sysfs none /sys\nexec 0</dev/console\nexec 1>/dev/console\nexec 2>/dev/console\n\nexec /bin/sh" > init
chmod +x init
mkdir dev etc home mnt proc sys
cd ./dev 
sudo mknod sda b 8 0 
sudo mknod console c 5 1

cd /tmp/src/linux-5.0.4/
export "ARCH=arm"
export "CROSS_COMPILE=arm-linux-gnueabihf-"
make menuconfig
make CONFIG_BLK_DEV_INITRD=y CONFIG_INITRAMFS_COMPRESSION_GZIP=y CONFIG_INITRAMFS_SOURCE="/tmp/rootfs" -j9

qemu-system-arm -m 256 -M virt -kernel /tmp/src/linux-5.0.4/arch/arm/boot/zImage -initrd /tmp/src/linux-5.0.4/usr/initramfs_data.cpio -nographic -append "console=ttyAMA0"
