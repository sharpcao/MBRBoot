#include "inc\os_io.h"

//__declspec(naked) 
void io_out8(uint port, uint value)
{
	__asm{
		mov edx, port
		mov eax, value
		out dx,al
		//ret
	}
}
__declspec(naked)
uchar io_in8(uint port)
{
	__asm{
		mov edx, [esp+4]
		mov eax,0
		in al,dx
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
void io_sti()
{
	__asm{
		sti
		ret
	}
}

__declspec(naked) 
void io_hlt()
{
	__asm{
		hlt
		ret
	}
}

__declspec(naked) 
void io_stihlt()
{
	__asm{
		sti
		hlt
		ret
	}
}

//__declspec(naked)  
uint io_load_eflags()
{
	uint result;
	__asm{
		pushfd 
		pop eax
		mov result,eax
		//ret
	}
	return result;
}

//__declspec(naked) 
void  io_store_eflags(uint eflags)
{
	__asm{
		//push dword ptr [esp+4]
		push eflags 
		popfd
		//ret
	}
}

__declspec(naked)
void  io_load_tr(uint idx)
{
	__asm{
		ltr [esp+4]
		ret
	}
}

__declspec(naked)
void task_switch(uint ip, uint cs)
{
	__asm{

		//jmp far fword ptr [esp+4]
		__emit 0xff
		__emit 0x6c
		__emit 0x24
		__emit 0x04

		ret
	}
}

__declspec(naked)
void jmp_cs(short int cs)
{
	__asm{
		add esp, 0x0c
		__emit 0xff
		__emit 0x5c
		__emit 0x24
		__emit 0xf4

	}
}




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



void load_gdt(OS_LOAD_GDT* ptr)
{
	__asm {
		mov	 eax, ptr
		lgdt [eax]
	};
}

void load_idt(OS_LOAD_IDT* ptr)
{
	__asm {
		mov	 eax, ptr
		lidt [eax]
	};
}

__declspec(naked) void cpu::get_regs(Reg* reg)
{
	
	__asm{
		push ebp
		push eax
		mov ebp, [esp+12]
		mov [ebp], eax
		mov [ebp+4], ebx
		mov [ebp+8], ecx
		mov [ebp+12], edx
		mov [ebp+16], esi
		mov [ebp+20], edi
		mov [ebp+24], cs
		mov word ptr [ebp+26], 0
		mov [ebp+28], ds
		mov word ptr [ebp+30], 0
		mov [ebp+32], es
		mov word ptr [ebp+34], 0
		mov [ebp+36], fs
		mov word ptr [ebp+38], 0
		mov [ebp+40], gs
		mov word ptr [ebp+42], 0
		pushfd
		pop eax
		mov [ebp+44], eax
		pop eax
		pop ebp
		ret
	}
}
