[BITS 32]

;
; Application startup routine, linked to every application
;

[global startup]

; Note, for some reason nasm generates FAF4 which is cli; hlt;

startup:
int 0x80
jmp $startup
