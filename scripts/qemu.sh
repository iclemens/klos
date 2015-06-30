#!/bin/sh

qemu -boot a -vga std -fda grub/floppy.img -hda harddisk.img
#qemu -boot a -fda grub/floppy.img -hda harddisk.img -nographic
#applications.iso
