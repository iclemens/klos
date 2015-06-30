#!/bin/bash

BOCHS="/usr/local/bin/bochs"
#BOCHS=bochs

$BOCHS 'boot: a' 'floppya: 1_44=grub/floppy.img, status=inserted' 'ata0-master: type=disk, path="harddisk.img", mode=flat, cylinders=10, heads=16, spt=63'

#$BOCHS 'boot: a' 'floppya: 1_44=grub/grub-use.img, status=inserted' 'floppyb: 1_44=grub/floppy.img, status=inserted' 'ata0-master: type=disk, path="harddisk.img", mode=flat, cylinders=10, heads=16, spt=63'

#$BOCHS="/usr/local/bin/bochs"
#$BOCHS 'boot: a' 'floppya: 1_44=grub/grub-use.img, status=inserted' 'floppyb: 1_44=grub/floppy.img, status=inserted' 'ata0-master: type=disk, path="harddisk.img", mode=flat, cylinders=10, heads=16, spt=63'

#bochs 'floppya: 1_44=kernel.iso, status=inserted' 'ata0-master: type=disk, path="harddisk.img", mode=flat, cylinders=20, heads=16, spt=63'
#bochs 'floppya: 1_44=freedos/fd.img, status=inserted' 'ata0-master: type=disk, path="harddisk.img", mode=flat, cylinders=20, heads=16, spt=63'
