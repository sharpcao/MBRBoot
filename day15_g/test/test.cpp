#include <iostream>

struct TSS32{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;

};

int main()
{
	std::cout << "sizeof(TSS32):" << sizeof(TSS32) << std::endl;

	__asm {
		__emit(0x32,0x33);
	}
	return 0;
}