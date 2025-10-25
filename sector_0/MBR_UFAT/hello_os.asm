				org 	0x8000
				jmp		_start

MSG_HELLO		db		'Hello OS!', 0


_start:
				mov		ax, 0x0003
				int     0x10
				mov		si,	MSG_HELLO
				cld		

.loop:
				lodsb
				test	al, al
				jz 		.done
				mov		ah, 0xe
				int  	0x10
				jmp     .loop
.done:
				hlt
				jmp		.done