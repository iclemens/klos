CC = gcc
CPP = g++
CXXFLAGS = -m32 -ffreestanding -nostdlib -nostdinc -Wall -fno-builtin -fno-rtti -fno-exceptions
CFLAGS = $(CXXFLAGS)
#LDFLAGS = -melf_i386 -T
LDFLAGS =
OBJS = bootstrap.o main.o interrupt.o memory.o console.o paging.o io.o interrupt_asm.o \
		pci.o kalloc.o ata.o paging_asm.o process.o gdt.o gdt_asm.o elf.o symbol.o \
		debug.o exceptions_asm.o exceptions.o vfs.o

default: all

exceptions_asm.o : exceptions.s
	nasm -f elf exceptions.s -o exceptions_asm.o

interrupt_asm.o : interrupt.s
	nasm -f elf interrupt.s -o interrupt_asm.o

paging_asm.o : interrupt.s
	nasm -f elf paging.s -o paging_asm.o

gdt_asm.o : gdt.s
	nasm -f elf gdt.s -o gdt_asm.o

bootstrap.o : bootstrap.s
	nasm -f elf bootstrap.s -o bootstrap.o

#$(file).o : $(file).c
#	echo Invoking C compiler
#	$(CC) -c $< -o $@ $(CFLAGS)

$(file).o : $(file).cpp
	echo Invoking C compiler
	$(CPP) -c $< -o $@ $(CXXFLAGS)

symbol-table.h:
	./symbol.sh

kernel.obj: $(OBJS) symbol-table.h
	ld $(LDFLAGS) ../kernel.ld -o kernel.obj $(OBJS)

all:
	touch symbol-table.h
	make kernel.obj
	make symbol-table.h
	make kernel.obj

clean:
	rm *.o
	rm kernel.obj

