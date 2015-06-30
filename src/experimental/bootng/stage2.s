[ORG 0x7E00]
[BITS 32]

;
; KlOS - Stage 2 boot-loader
;

; Setup segment selectors
mov eax, 0x10
mov ds, eax
mov es, eax
mov fs, eax
mov gs, eax
mov ss, eax
a32

; Clear screen and print banner
call clear_screen

push klos_banner
call print_string
add esp, 4

; Actually read OS from disk

;; void ata_read_sectors(uint32_t dest, uint32_t address, uint32_t count)
push 1
push 0
push 0x100000
call ata_read_sectors
add esp, 12

; Read first LBA address of first parition into memory
mov eax, dword[0x1001C6]
mov dword[first_sector], eax

push 1
push eax
push 0x100000
call ata_read_sectors
add esp, 12

; first_data_sector = fat_boot->reserved_sector_count + (table_count * size_size_16)
xor eax, eax
mov ax, word[0x100016]	; Sectors per FAT
mov dl, byte[0x100010]  ; Number of FATs
mul dl
add ax, word[0x10000E]	; Number of reserved sectors
add eax, dword[first_sector]

push eax
call print_directory
add esp, 4




hlt

%include "disk.s"
%include "console.s"
%include "fat.s"

first_sector: dd 0
x_cursor: db 0
y_cursor: db 0
klos_banner: db "KlOS Stage 2 Bootloader\nReading OS from disk...\n\n", 0

times 2048-($-$$) db 0
