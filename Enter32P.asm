			org		8000h
			jmp  	start_code

gdt_size: 	dw 32-1			 ;GDT Table size, total bytes minus 1
gdt_base:	dd 0x00007e00	 ;GDT base address

start_code:

	;初始化GDT表
			mov		si,				7e00h
			
			;Create #0 description, it should be empty
			mov		dword [si],		0h
			mov		dword [si+4],	0h

			;1st description for CS
			mov		dword [si+8],	0x8000ffff
			mov		dword [si+12],	0x00409800

			;2nd description for DS and ES
			mov		dword [si+16],	0x0000ffff
			mov		dword [si+20],	0x00c09200

			;3rd description for SS
			mov		dword [si+24],	0x00007a00
			mov		dword [si+28],	0x00409600

	;Load 48bit GDT
			lgdt	[gdt_size]						

	;Open A20 address
			in 		al,				0x92    		
			or 		al,				0000_0010B
			out 	0x92,			al

	;The interrupts should be prohibited now
			cli	

	;Turn on the protection mode
			mov 	eax,cr0  	
			or 		eax,1
			mov 	cr0,eax

	;Set DS\ES\SS\ESP, jump to Entry Point
			mov		ax, 00000000000_10_000b
			mov		ds,	ax
			mov		ax,	00000000000_11_000b
			mov		ss, ax
			mov		esp, 0x7c00
			mov		es, ax
			jmp 	dword 0x0008:0x200 ;cs=8=1000b，The first Selector


times  512 -($-$$) db 0
	


