[BITS 32]

			mov 	eax, 'h'
			call	print_char
			mov		eax, 'e'
			call	print_char
			mov		eax, 'l'
			call	print_char
			mov		eax, 'l'
			call	print_char
			mov		eax, 'o'
			call	print_char
			retf

print_char:
			push	eax
			call	1*8:0x10af18
			ret



			;call far [esp + 4]
