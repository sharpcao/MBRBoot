#include "inc\console_api.h"


API_ADDR API_Entry[10] ={
	(API_ADDR)api_jmp_func,
	(API_ADDR)api_print_char2,
	(API_ADDR)api_print_char2,
};

__declspec(naked) extern "C"
void  __stdcall api_jmp_func()   //@ecx = api_idx
{
	__asm{
		push 	ebx;
		push 	ds
		mov 	eax, 2*8;
		mov 	ds, ax
		shl 	ecx, 2
		lea 	ebx, dword ptr API_Entry
		add   	ebx, ecx
		mov   	ecx, [ebx]
		pop     ds
		pop 	ebx
		jmp    	ecx
	}
}


extern void* p_func1;
extern uint SYSTEM_ESP ;

__declspec(naked) 
void  __stdcall api_print_char1(uchar c)
{
	__asm{
		mov eax, [esp + 8]
		and eax, 0xff
		mov ecx, p_func1
		push eax
		call ecx
		retf 4
	}
}

__declspec(naked)
void __stdcall api_print_char2(uchar c)
{
	
	__asm{
		push ds
		push es
		mov  eax, 2*8
		mov  ds, ax
		mov  es, ax
		mov  ecx, SYSTEM_ESP 
		sub  ecx, 12
		mov  eax,[esp + 12]
		mov  [ecx], eax
		mov  eax, esp
		mov  [ecx + 4], eax
		mov  eax, ss
		mov  [ecx + 8], eax
		mov  eax, 3*8
		mov  ss, ax
		mov  esp, ecx

		mov  ecx, p_func1
		call ecx
		pop  ecx
		pop  ss
		mov  esp, ecx
		pop  es
		pop  ds
		retf 4

	}

}