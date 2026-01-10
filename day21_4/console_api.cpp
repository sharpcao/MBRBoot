#include "inc\console_api.h"
#include "inc\console.h"

void __stdcall (*volatile p_func1)(uchar c) = console_print_char;

API_ADDR API_Entry[] ={
	0,
	(API_ADDR)console_print_char,
};




/*
void test_init()
{
	API_Entry[1] = (API_ADDR)p_func1;
}
*/

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



//extern uint SYSTEM_ESP ;


extern ConsoleLayer* cur_ConsoleLayer;

uint int03_print_message()
{
	if (cur_ConsoleLayer){
		cur_ConsoleLayer->append_string("#0D,General Protected Exception\n");
		//cur_ConsoleLayer->append_prefix(true);
	}
	return 1;
}


void __stdcall console_print_char(uchar c)
{
	extern  API_ADDR API_Entry[];
	cur_ConsoleLayer->append_char(c);
	cur_ConsoleLayer->refresh();
	API_Entry[0] = 8;
}

