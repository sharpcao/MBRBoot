int main()
{


	for (int i = 0xa0000; i<=0xaffff;++i){
		*((char*)i) = i & 0x0f;

	}

	asm {
loop:
	hlt
	jmp loop

	}
}

 