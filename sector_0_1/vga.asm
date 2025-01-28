org 0x8000
mov al, 0x13
mov ah, 0x00
int	10h

fin:
	hlt
	jmp fin