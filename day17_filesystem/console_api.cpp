#include "inc\console_api.h"



API_ADDR API_Entry[10] ={
	(API_ADDR)api_call_func,
	(API_ADDR)api_print_char1
};

__declspec(naked) extern "C"
void  __stdcall api_call_func()   //@ecx = api_idx
{
	__asm{
		push 	ebx;
		shl 	ecx, 2
		lea 	ebx, dword ptr API_Entry
		add   	ebx, ecx
		mov   	ecx, [ebx]
		pop 	ebx
		jmp    	ecx
	}
}


extern void* p_func1;

__declspec(naked) 
void  __stdcall api_print_char1(uchar c)
{
	asm{
		mov eax, [esp + 8]
		and eax, 0xff
		mov ecx, p_func1
		push eax
		call ecx
		retf 4
	}
}
