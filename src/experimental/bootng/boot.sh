nasm stage1.s &&
nasm stage2.s &&
cat stage1 stage2 > floppy.img &&
qemu-system-x86_64 -fda floppy.img -hda testdisk.img -boot a -net none

#/usr/local/bin/bochs 'floppya: 1_44=floppy.img, status=inserted'

# dd if=testfile of=testdisk.img bs=512 count=50
# bximage -hd -size=10 testdisk.img

# /usr/local/bin/bochs 'floppya: 1_44=floppy.img, status=inserted' 'ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14' 'ata0-master: type=disk, path="testdisk.img", mode=flat, cylinders=20, heads=16, spt=63'

