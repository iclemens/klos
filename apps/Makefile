CC = gcc
CPP = g++
ASM = nasm

CXXFLAGS=-fno-stack-protector -m32
CFLAGS = $(CXXFLAGS)
LDFLAGS = -melf_i386

default: harddisk.img

startup.o: startup.s
	nasm -f elf startup.s -o startup.o

libc.o: libc.s
	nasm -f elf libc.s -o libc.o

helloworld.o: helloworld.c
	gcc -c helloworld.c -o helloworld.o $(CFLAGS)

helloworld.app: helloworld.o startup.o libc.o
	mkdir -p binaries
	ld $(LDFLAGS) -s -T helloworld.ld -o binaries/helloworld.app startup.o libc.o helloworld.o

hang.o: hang.s
	nasm -f aout hang.s -o hang.o

hang.app: hang.o
	mkdir -p binaries
	ld $(LDFLAGS) -s -T helloworld.ld -o binaries/hang.app hang.o

binaries: helloworld.app hang.app

harddisk.img: helloworld.app
	./make-images.sh

applications.iso: binaries
	genisoimage -input-charset iso8859-1 -o ../applications.iso binaries

all: harddisk.img
