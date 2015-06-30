
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void ata_read_sectors(uint32_t dest, uint32_t address, uint32_t count)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ata_read_sectors:
  push ebp
  mov ebp, esp

  push dword[ebp + 16]
  push dword[ebp + 12]
  call ata_request_sectors
  add esp, 8

  push dword[ebp + 16]
  push dword[ebp + 8]
  call ata_transfer_sectors
  add esp, 8

  mov esp, ebp
  pop ebp
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void ata_request_sectors(uint32_t address, uint32_t count)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ata_transfer_sectors:
  push ebp
  mov ebp, esp

  
  ata_transfer_sectors_next:

  push dword[ebp + 8]
  push ada_debug_store_str
  call print_string
  add esp, 8 

  push ata_debug_poll_start
  call print_string
  add esp, 4

  ; Poll until ready
  mov dx, 0x1F7

  ata_transfer_sectors_poll:  
  in al, dx
  test al, 0x80
  jne ata_transfer_sectors_poll
  test al, 0x08
  jne ata_transfer_sectors_ready
  jmp ata_transfer_sectors_poll

  ata_transfer_sectors_ready:

  push ata_debug_poll_complete
  call print_string
  add esp, 4

  mov edi, [ebp + 8]
  mov ecx, 128
  mov dx, 0x1F0
  rep insd

  ; Increase data-pointer
  mov ebx, dword[ebp + 8]
  add ebx, 0x200
  mov dword[ebp + 8], ebx

  mov ebx, dword[ebp + 12]
  dec ebx
  mov dword[ebp + 12], ebx

  cmp ebx, 0
  ja ata_transfer_sectors_next

  mov esp, ebp
  pop ebp
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void ata_request_sectors(uint32_t address, uint32_t count)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ata_request_sectors:
  push ebp
  mov ebp, esp

  mov ebx, [ebp + 8]	; LBA48 address
  mov ecx, [ebp + 12]	; Count

  ; Does nothing
  mov dx, 0x01F1
  mov al, 0x00
  out dx, al
  
  ; Number of sectors requested
  mov dx, 0x1F2
  mov al, cl
  out dx, al

  ; Bytes 0 through 3
  mov eax, ebx
  mov dx, 0x1F3
  mov al, al
  out dx, al

  mov eax, ebx
  shr eax, 8
  mov dx, 0x1F4
  out dx, al

  mov eax, ebx
  shr eax, 16
  mov dx, 0x1F5
  out dx, al

  ; Send stuff
  mov dx, 0x1F6
  mov al, 0xE0
  out dx, al

  ; READ_SECTORS
  mov dx, 0x1F7
  mov al, 0x20
  out dx, al

  push dword[ebp + 8]
  push dword[ebp + 12]
  push ata_debug_str
  call print_string
  add sp, 12

  mov esp, ebp
  pop ebp
  ret

ata_debug_str: db "Requested %x sectors at LBA48 address %x\n", 0
ada_debug_store_str: db "Storing sectors at address %x\n", 0
ata_debug_poll_start: db "Polling drive status\n", 0
ata_debug_poll_complete: db "Drive is ready, starting transfer\n", 0
