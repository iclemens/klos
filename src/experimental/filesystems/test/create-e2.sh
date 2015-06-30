#!/bin/bash

dd if=/dev/zero of=testfs-ext2 bs=1024 count=30720
losetup /dev/loop0 testfs-ext2
mkfs -t ext3 -m 1 -v /dev/loop0
mount -t ext3 /dev/loop0 /mnt/vfs



umount /mnt/vfs
losetup -d /dev/loop0