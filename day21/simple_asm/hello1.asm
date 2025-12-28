[BITS 32]
			int   	0x40
			mov   	dword [api_entry], eax

			mov 	eax, 'h'
			call    print_char
			mov 	eax, 'e'
			call 	print_char
			mov 	eax, 'l'
			call 	print_char
			mov 	eax, 'l'
			call 	print_char
			mov 	eax, 'o'
			call 	print_char

			retf




print_char:
			push	ecx
			push	eax
			mov		ecx, 1
			call  	call_api
			pop		ecx
			ret
call_api:
			mov 	eax, [esp]
			push	eax
			mov		eax, cs
			mov		[esp+4], eax
			mov		eax, 1*8
			push	eax
			mov    	eax, [api_entry]
			push	eax
			retf
			

api_entry	dw		0h;