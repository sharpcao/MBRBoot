#include "bootpack.h"

int main()
{

	CPalette pal;
	pal.init_palette();

	char* vga_mem_base = (char*)0xa0000;

	for(uint i=0; i<= 0xffff; ++i)
	{
		*(vga_mem_base + i) = i & 0x0f;
	}
	
	for(;;)
	{
		asm{
			hlt;
		}
	}

}