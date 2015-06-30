[BITS 32]

[global gdt_reload_asm]
[extern gdtp]

gdt_reload_asm:
    lgdt [gdtp]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:continue

continue:
    ret