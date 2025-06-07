#ifndef OS_IO_H
#define OS_IO_H
using uint=unsigned int;
using uchar=unsigned char;
enum PIC {
		PIC0_ICW1=0x0020,
		PIC0_OCW2=0x0020,
		PIC0_IMR=0x0021,
		PIC0_ICW2=0x0021,
		PIC0_ICW3=0x0021,
		PIC0_ICW4=0x0021,
		PIC1_ICW1=0x00a0,
		PIC1_OCW2=0x00a0,
		PIC1_IMR=0x00a1,
		PIC1_ICW2=0x00a1,
		PIC1_ICW3=0x00a1,
		PIC1_ICW4=0x00a1
	};

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
void io_sti()
{
	__asm{
		sti
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



__declspec(naked) 
void io_store_eflags(uint eflags)
{
	__asm{
		push dword ptr [esp+4] 
		popfd
		ret
	}
}

__declspec(naked)
void io_load_tr(uint idx)
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



#endif

