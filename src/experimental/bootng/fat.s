
; print_directory(uint32_t directory sector)
print_directory:
  push ebp
  mov ebp, esp

  sub esp, 4	; Reserve 4 bytes for counter variable

  ; Read directory sector
  push 1
  push dword[ebp + 8]	; Directory sector
  push 0x100000
  call ata_read_sectors
  add esp, 12

  mov edi, 0x100000

  next_dir:
  cmp edi, 0x100200
  jae done

  mov eax, [edi + 0x0B]
  and eax, 0x1F
  cmp eax, 0x00
  je valid_dir
  valid_dir_continuation:

  add edi, 0x20
  jmp next_dir


  valid_dir:

  mov dword[ebp - 4], 0

  more_chars:
  mov ecx, [ebp - 4]
  mov eax, [edi + ecx]
  add ecx, 1
  mov [ebp - 4], ecx

  ; Print character
  push edi
  push eax
  call print_char
  add esp, 4
  pop edi

  mov ecx, [ebp - 4]
  cmp ecx, 11
  jb more_chars

  push edi
  push '|'
  call print_char
  add esp, 4

  push 0xA
  call print_char
  add esp, 4
  pop edi

  jmp valid_dir_continuation


  done:
  mov esp, ebp
  pop ebp
  ret

