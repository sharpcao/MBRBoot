;   Enter 32-bit protection mode, reload the PEOS to proper location
; 	
;	nasm load_peos.asm -o sector_1.bin
;
;	Author:Sharp.Cao
;	Date:2025/1/26
;-----------------------------------------------------------------------------------------
				org	0x8000
PE_Base			equ 0x8000
PE_Head 		equ	PE_Base + 0x200
EntryPoint		equ PE_Head + 0x28
ImageBase		equ PE_Head + 0x34 
CodeVirtualAddr equ PE_Head + 0x104 
CodeRawSize		equ PE_Head + 0x108 
CodeRawPointer	equ PE_Head + 0x10c 
DataVirtualAddr equ PE_Head + 0x12c 
DataRawSize		equ PE_Head + 0x130 
DataRawPointer	equ PE_Head + 0x134 

;-----------------------------------------------------------------------------------------
[bits 16]
				jmp  	_start

gdt_size: 		dw 8*4-1			 ;GDT Table size, total bytes minus 1
gdt_base:		dd GDT_BASE_ADDR	 ;GDT base address

GDT_BASE_ADDR:
GDT_0:			dd 0h, 0h
GDT_1_CS:		dd 0x0000ffff, 0x00cf9a00
GDT_2_DS:		dd 0x0000ffff, 0x00cf9200
GDT_3_SS:		dd 0x00000000, 0x00409600


_start:

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
			mov		es, ax
			mov		ax,	00000000000_11_000b
			mov		ss, ax
			mov		esp, 0x7c00
			
			jmp 	dword 0x0008:_copy_image ; cs=8=1000b，The first Selector

;-----------------------------------------------------------------------------------------
[bits 32]
;copy_image hard code
;			image_base	equ    0x100000
;			code_rva	equ		 0x1000
;			data_rva	equ		 0x2000
;			entry_point	equ	 0x1008
;			org_base	equ		 0x8000
;			code_off	equ		  0x600
;			code_size	equ		  0x200
;			data_off	equ		  0x800
;			data_size	equ		  0x200

;			mov 	esi, org_base + code_off
;			mov 	edi, image_base + code_rva
;			mov		ecx, code_size / 4
;			rep		movsd
;			mov		esi, org_base + data_off
;			mov		edi, image_base + data_rva
;			mov 	ecx, data_size / 4
;			rep		movsd
;			jmp		dword 0x0008:image_base + entry_point

_copy_image:
			mov		ebx, [ImageBase]		;ImageBase
;copy code segment virtual address
			mov   	esi, [CodeRawPointer]
			add 	esi, PE_Base
			mov		edi, [CodeVirtualAddr]
			add   	edi, ebx
			mov		ecx, [CodeRawSize]
			shr		ecx, 2
			rep		movsd			
;copy data segment to virtual address		
			mov   	esi, [DataRawPointer]
			add  	esi, PE_Base
			mov     edi, [DataVirtualAddr]
			add  	edi, ebx
			mov  	ecx, [DataRawSize]
			shr		ecx, 2
			rep 	movsd
;jump to EntryPoint
			mov 	eax, [EntryPoint]		
			add 	eax, ebx
			jmp     eax

times  512 -($-$$) db 0
	


