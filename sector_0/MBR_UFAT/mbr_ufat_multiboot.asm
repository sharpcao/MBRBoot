				org		0x7c00
[bits 16]

STACK_TOP		equ		0x7000
BUF_META		equ		0x7000
	META_FILE 	equ		BUF_META + 0x20
	META_FMAP 	equ		BUF_META + 0x30
	META_FDAT 	equ		BUF_META + 0x40


BUF_FILE		equ		0x7200
BUF_FMAP		equ		0x7400

MEM_LOAD_BASE   equ		0x8000
OS_MAX_SIZE		equ		128 			;128kb

				jmp		0:_start


;Global data
LAST_LOAD		dd		0
DRIVER			db 		0
MSG_BOOT		db   	'BOOT:',0


_start:
;Initial regs
				cli
				xor		ax, ax
				mov		ds, ax
				mov		es, ax
				mov		fs, ax
				mov		gs, ax
				mov		ss, ax
				mov		bx, ax
				mov		cx, ax
				mov		si, ax
				mov		di, ax
				mov		bp, ax
				mov		sp, STACK_TOP		
				sti	
				mov		byte [DRIVER], dl
				mov		ax, 0x0003
				int   	0x10

;Load disk meta
				push 	BUF_META / 16
				push 	1
				push 	0
				push  	1
				call  	read_disk_lba

;Load file records to BUF_FILE
				mov		ax, [META_FILE + 0x8]		;start address
				mov		dx, [META_FILE + 0xa]
				call	convert_serial
				push 	BUF_FILE / 16
				push   	1
				push 	dx
				push 	ax
				call	read_disk_lba

;Print 10 files
				mov		si, BUF_FILE
				call	print_files
				mov 	cl, al
				add 	cl, '0'
;Choose file to boot 0~9
				mov		si, MSG_BOOT
				call	print_str
.choose_file_loop:
				mov		ah, 0
				int  	0x16
				cmp		al, '0'
				jl 		.choose_file_loop
				cmp		al, cl
				jge  	.choose_file_loop
				sub  	al, '0'
				mov 	ah,  0
				call    get_file_start_chunk

;Load from start chunk idx
				mov		cx, 0
				mov		di, MEM_LOAD_BASE / 16

.load_file:
				push 	di
				push 	dx
				push   	ax
				call    read_file_chuck
				inc  	cx
				add  	di, 1024 / 16
				cmp		cx, OS_MAX_SIZE
				jge 	.done

				call    find_next_idx
				cmp		ax, 0xffff
				jnz		.load_file
				cmp  	dx, 0xffff
				jnz 	.load_file

.done:
				;hlt
				jmp 	MEM_LOAD_BASE



;--------------------------------------------------------------------------------------------
; input @ax = user choose num
; ouput 
;		@ax = file start chunk low
;		@dx = file start chunk high
get_file_start_chunk:
				push  	di
				mov		di, ax
				shl 	di, 5
				mov  	ax, [BUF_FILE + di + 0x18]
				mov   	dx, [BUF_FILE + di + 0x1a]  	
				pop 	di
				retn
				
;--------------------------------------------------------------------------------------------
; input
; 		@ax = current idx low
;		@dx = current idx high
; output
; 		@ax = next idx low
;		@dx = next idx high
find_next_idx:
				push 	bx
				push 	cx
				push    bp
				sub		sp, 8

				mov		bp, sp
				mov		[bp + 4], ax
				mov		[bp + 6], dx
				mov		ax, [META_FMAP + 0x08]
				mov		dx, [META_FMAP + 0x0a]
				call 	convert_serial
				mov		[bp], ax
				mov		[bp + 2], dx

				mov		ax, [bp + 4]
				mov		dx, [bp + 6]
				mov		bx, ax
				and		bx, 0x007f
				shl		bx, 2
				mov 	cx, 7
				call 	dxax_shr
				push    word [bp + 2]
				push 	word [bp]
				call	dxax_add
				cmp		ax, [LAST_LOAD]
				jnz		.load_fmap_sect
				cmp		dx, [LAST_LOAD + 2]
				jnz		.load_fmap_sect
				jmp 	.has_load
.load_fmap_sect:
				push  	BUF_FMAP / 16 				;convert to segment
				push    1   						
				push  	dx
				push 	ax
				call	read_disk_lba
				mov		[LAST_LOAD], ax
				mov		[LAST_LOAD + 2], dx

.has_load:
				mov  	ax, [BUF_FMAP + bx]
				mov     dx, [BUF_FMAP + bx + 2]

				add     sp, 8
				pop    	bp
				pop  	cx
				pop 	bx
				retn
;--------------------------------------------------------------------------------------------
;input
; 	@si = file record start
;output
; 	@ax = actual file
print_files:
				push    si
				push   	cx
				xor		cx,	cx
.loop:
				mov		al, [si]
				test  	al, al
				jz		.done
				mov		al, cl
				add     al, 0x30
				call	print_char
				mov		al, ' '
				call	print_char
				call  	print_str
				call	print_newline
				add 	si, 0x20
				inc    	cx
				cmp     cx, 9
				jle  	.loop
				
.done:
				mov 	ax, cx
				pop     cx
				pop     si
				retn

;--------------------------------------------------------------------------------------------
; input  @dx:@ax 
; output @dx:@ax = @dx:@ax / 512  
convert_serial:
				mov		al, ah
				mov		ah, dl
				shr		dx, 8
				shr		dx, 1
				rcr    	ax, 1
				ret

;--------------------------------------------------------------------------------------------
; input @1 = low bit, @2 = high bit
; ouput @dx:@ax = @dx:@ax + input
dxax_add:
				push	bp
				mov		bp, sp
				add 	ax, word [bp + 4]
				adc 	dx, word [bp + 6]
				pop		bp
				retn 	4


;--------------------------------------------------------------------------------------------
; input @cx = shift right num
; ouput @dx:@ax = @dx:@ax >> @cx
dxax_shr:
.loop:
				test	cx, cx
				jz 		.done
				shr  	dx, 1
				rcr    	ax, 1
				dec     cx
				jmp 	.loop
.done:
				retn
;--------------------------------------------------------------------------------------------
; input
; 	@1 = chunk_idx_low
;	@2 = chunk_idx_high
; 	@3 = memory_seg
; output 
;	NA
read_file_chuck:
				pusha
				mov 	bp, sp


				mov		ax, [META_FDAT + 0x08]
				mov		dx, [META_FDAT + 0x0a]			; FDAT base address
				call	convert_serial
				mov		cx, [bp + 0x12]					; chunk_idx
				mov		bx, [bp + 0x14]
				shl		cx, 1 
				rcl     bx, 1
				add 	ax, cx
				adc 	dx, bx

				push  	word [bp + 0x16]
				push 	2				
				push 	dx
				push 	ax
				call  	read_disk_lba

				mov		sp, bp
				popa
				retn	6
;--------------------------------------------------------------------------------------------
;+0x12  @1 = serial_low
;+0x14  @2 = serial_high
;+0x16  @3 = nsector
;+0x18  @4 = memory_seg
read_disk_lba:
				pusha 	
				mov		bp, sp
				sub		sp, 0x10

				mov		si, sp
				mov		word  [si], 0x0010
				mov		ax,  [bp + 0x16]
				mov		[si + 2], ax			; nsector
				mov		word [si + 4], 0 		; memory offset
				mov 	ax, [bp + 0x18]
				mov 	[si + 6], ax			; memory seg
				mov		ax, [bp + 0x12]			
				mov		[si + 8], ax			; serial_low
				mov		ax, [bp + 0x14]
				mov		[si + 10], ax			; serial_high
				xor		ax, ax
				mov		[si + 12], ax
				mov		[si + 14], ax
				mov		dl, [DRIVER]
				mov		ah, 0x42
				int     0x13

				mov 	sp, bp
				popa 	
				retn	8

;--------------------------------------------------------------------------------------------
;@si = string
print_str:
				pusha
				cld
.loop:
				lodsb
				cmp 	al, 0
				jz		.done
				call	print_char
				jmp 	.loop
.done:
				popa
				retn

;--------------------------------------------------------------------------------------------
;input @al = char 
print_char:
				mov		ah, 0x0e
				int 	0x10
				retn

;--------------------------------------------------------------------------------------------
print_newline:
				mov		al, 0x0d
				call	print_char
				mov		al, 0x0a
				call	print_char
				retn

;--------------------------------------------------------------------------------------------
				times	510 - ($ -$$) 	db 0
				dw 		0xAA55
				