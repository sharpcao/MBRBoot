					org  		0x8000
					jmp		_start
[bits 16]
struc  VgaInfoBlock
  	.VESASignature   resb   4      		; 4 signature bytes
      .VESAVersion     resw   1     		; VESA version number
      .OEMStringPtr    resd   1          ; Pointer to OEM string
      .Capabilities    resb   4 		; capabilities of the video environment
      .VideoModePtr    resd   1          ; pointer to supported Super VGA modes
      .TotalMemory     resw   1          ; Number of 64kb memory blocks on board
      .Reserved        resb   236 		; Remainder of VgaInfoBlock
endstruc

_start:
				mov		ax, 0
				mov		ds, ax
				mov		es, ax
				mov		fs, ax
				mov		gs, ax
				mov		ss, ax
				mov		sp, 0x7c00

;---------------------------------------------------------------------------------
;Function 00h - Return Super VGA Information
;        Input:  AH = 4Fh        Super VGA support
;                AL = 00h        Return Super VGA information
;                ES:DI = Pointer to buffer
;
;        Output: AX = Status
	
				mov 		ax, 0x4f00
				mov		di, 0x9000
				int 		0x10

				call        print_newline
				mov  		si, [0x9000 + VgaInfoBlock.OEMStringPtr]
				mov     	ax, [0x9000 + VgaInfoBlock.OEMStringPtr + 2]
				call    	print_DS_SI_string

.wait:
				hlt
				jmp 		.wait
;---------------------------------------------------------------------------------------------
print_DS_SI_string:
				push 		ds
				mov 		ds, ax
				call   	print_str
				pop 		ds
				retn
;--------------------------------------------------------------------------------------------
;@si = string
print_str:
				pusha
				cld
.loop:
				lodsb
				cmp 		al, 0
				jz		.done
				call		print_char
				jmp 		.loop
.done:
				popa
				retn

;--------------------------------------------------------------------------------------------
;input @al = char 
print_char:
				mov		ah, 0x0e
				int 		0x10
				retn

;--------------------------------------------------------------------------------------------
print_newline:
				mov		al, 0x0d
				call		print_char
				mov		al, 0x0a
				call		print_char
				retn

