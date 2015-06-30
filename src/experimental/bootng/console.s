
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void print_numeral(uint32_t number)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

print_numeral:
  push ebp
  mov ebp, esp
  sub esp, 4		; We've got 1 local variables

  push '0'
  call print_char
  add esp, 4
  
  push 'x'
  call print_char
  add esp, 4

  ; Put number of bytes to print in local variable
  mov ecx, 8
  mov [ebp - 4], ecx
    
  print_numeral_repeat:

  mov edx, [ebp + 8]
  mov ecx, [ebp - 4]
  and ecx, 0xFF

  sub cl, 1
  shl cl, 2

  shr edx, cl
  and edx, 0xF
  cmp edx, 0x9
  jbe print_numeral_number
  ja print_numeral_letter
  
  print_numeral_continuation:

  mov ecx, [ebp - 4]
  sub ecx, 1
  mov [ebp - 4], ecx

  cmp ecx, 0
  ja print_numeral_repeat

  mov esp, ebp
  pop ebp
  ret

  print_numeral_number:
  xor eax, eax
  mov al, dl
  add al, '0'
  push eax
  call print_char
  add sp, 4
  jmp print_numeral_continuation

  print_numeral_letter:
  xor eax, eax
  mov al, dl
  add al, 'A'
  sub al, 0xA
  push eax
  call print_char
  add sp, 4
  jmp print_numeral_continuation


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void print_char(uint8_t c)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

print_char:
  push ebp
  mov ebp, esp
 
  mov eax, [ebp + 8]
  cmp eax, 0x0A
  je print_char_progress_line

  ; Compute position of character on screen
  xor eax, eax
  mov ax, word[cursor_y]
  mov dl, 80
  mul dl
  add ax, word[cursor_x]
  shl eax, 1
  add eax, 0xB8000
  mov edi, eax

  mov eax, [ebp + 8]
  stosb

  mov cx, word[cursor_x]
  add cx, 1
  mov word[cursor_x], cx

  cmp cx, 80
  ja print_char_progress_line

  print_char_continuation:
  call move_cursor

  mov esp, ebp
  pop ebp
  ret

  print_char_progress_line:
  mov cx, 0
  mov word[cursor_x], 0

  mov cx, word[cursor_y]
  add cx, 1
  mov word[cursor_y], cx

  cmp cx, 24
  ja print_char_scroll

  jmp print_char_continuation

  print_char_scroll:
  mov cx, 24
  mov word[cursor_y], cx
  jmp print_char_continuation

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void print_string(char *)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

print_string:
  push ebp
  mov ebp, esp
  sub esp, 4

  ; Setup parameter pointer
  mov eax, 12
  mov [ebp - 4], eax

  ; Walk over string (pointer in ebp + 8) and print characters
  print_string_more:
  mov ebx, [ebp + 8]

  xor eax, eax
  mov al, byte[ebx]
  cmp al, 0
  je print_string_terminate

  cmp al, '\'
  je print_string_escape

  cmp al, '%'
  je print_string_variable
  
  and eax, 0xFF
  push eax
  call print_char
  add sp, 4

  mov ebx, [ebp + 8]  
  inc ebx
  mov [ebp + 8], ebx

  jmp print_string_more

  ; Handle printing of variables
  print_string_variable:
  mov ebx, [ebp + 8]
  inc ebx ; Skip over %
  mov al, byte[ebx]
  inc ebx
  mov [ebp + 8], ebx
  
  cmp al, 'x'
  je print_string_var_hex

  jmp print_string_more

  print_string_var_hex:
  mov ebx, [ebp - 4]	; Current parameter
  mov ecx, [ebp + ebx]
  add ebx, 4
  mov [ebp - 4], ebx

  push ecx
  call print_numeral
  add sp, 4
  jmp print_string_more

  ; Handle special escape characters (e.g. \n)
  print_string_escape:
  mov ebx, [ebp + 8]
  inc ebx
  mov al, byte[ebx]
  inc ebx
  mov [ebp + 8], ebx
  
  cmp al, 'n'
  je print_string_newline

  jmp print_string_more

  print_string_newline:
  mov ebx, 0xA
  push ebx
  call print_char
  add esp, 4

  jmp print_string_more  


  print_string_terminate:

  mov esp, ebp
  pop ebp
  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void clear_screen()
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

clear_screen:
  push ebp
  mov ebp, esp
     
  mov ebx, 0xB8000	; 0xB800 + (80 x 25)
  mov edi, ebx

  mov ecx, 0x3E8
  mov eax, 0x07200720

  rep stosd

  mov word[cursor_x], 0
  mov word[cursor_y], 0
  call move_cursor

  mov esp, ebp
  pop ebp

  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void move_cursor(x, y)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

move_cursor:
  push ebp
  mov ebp, esp

  ; Y * Width  + X
  xor dh, dh
  mov dl, 80		; Screen width
  mov ax, word[cursor_y]
  mul dx
  add ax, word[cursor_x]
 
  mov bx, ax

  mov dx, 0x3D4
  mov al, 0x0E
  out dx, al

  mov dx, 0x3D5
  mov al, bh
  out dx, al

  mov dx, 0x3D4
  mov al, 0x0F
  out dx, al

  mov dx, 0x3D5
  mov al, bl
  out dx, al

  mov esp, ebp
  pop ebp
  ret

cursor_x: dw 0
cursor_y: dw 0
