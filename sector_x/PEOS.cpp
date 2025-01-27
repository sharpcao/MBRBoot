int main()
{


	for (int i = 0xb8000; i<=0xbffff;){
		*((char*)i++) = 'C';
		*((char*)i++) = 0x1c;

	}

	asm {
loop:
	hlt
	jmp loop

	}
}

 