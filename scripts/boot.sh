nasm -o bootsect.o bootsect.s &&
cat bootsect.o kernel/kernel.obj > floppy.img &&
#qemu -fda floppy.img
/usr/local/bin/bochs 'floppya: 1_44=floppy.img, status=inserted'
