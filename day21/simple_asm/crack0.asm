[BITS 32]
			int   	0x40
			mov   	dword [api_entry], eax
			mov		dword [0x100000], eax



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