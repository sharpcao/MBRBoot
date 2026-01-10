[BITS 32]
			
			mov 	edx, 'h'
			mov		ecx, 1
			call	print_char
			mov		edx, 'e'
			mov		ecx, 1
			call	print_char
loop:
			jmp loop


print_char:
			int 40h
			ret

exit_app:
			mov ecx, 0
			int 40h

