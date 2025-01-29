#ifndef OS_IO_H
#define OS_IO_H

using uint = unsigned int;

//__declspec(naked) 
void io_out8(uint port, uint data)
{
	__asm{
		mov edx, port
		mov eax, data
		out dx, al 
		//ret
	}
}

__declspec(naked)  
uint io_load_eflags()
{
	__asm{
		pushfd 
		pop eax
		ret
	}
}

__declspec(naked) 
void io_cli()
{
	__asm {
		cli
		ret
	}
}

__declspec(naked) 
void io_store_eflags(uint eflags)
{
	asm{
		push dword ptr [esp+4] 
		popfd
		ret
	}
}



#endif

