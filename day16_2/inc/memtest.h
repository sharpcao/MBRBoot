#ifndef MEMTEST_H
#define MEMTEST_H

using uint = unsigned int;
__declspec(naked)
uint load_cr0()
{
	__asm{
		mov eax,cr0
		ret
	}
}

__declspec(naked)
void store_cr0(uint v)
{
	__asm{
		mov eax,[esp+4]
		mov cr0,eax
		ret
	}
}



uint memtest(uint start, uint end)
{
	constexpr uint pat0 = 0xaa55aa55;
	constexpr uint pat1 = 0x55aa55aa;
	constexpr uint CR0_CACHE_DISABLE = 0x60000000;

	uint cr0 = load_cr0();
	cr0 |= CR0_CACHE_DISABLE;
	store_cr0(cr0);
	uint* p;
	for(p = (uint*)start; p != (uint*)end; p+=0x1000)
	{
		uint old = *p;
		*p = pat0;
		*p ^= 0xffffffff;
		if(*p != pat1){
			*p = old;
			break;
		}else{
			*p ^= 0xffffffff;
			if(*p != pat0){
				*p = old;
				break;
			} 
		}
		*p = old;
	}

	cr0 = load_cr0();
	cr0 &= ~CR0_CACHE_DISABLE;
	store_cr0(cr0);

	return (uint)p;
}


#endif