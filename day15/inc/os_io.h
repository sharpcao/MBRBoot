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



struct SEGMENT_DESCRIPTOR
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
	void set(uint limit, uint base, uint ar);
	
};


struct GATE_DESCRIPTOR
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
	void set(uint offset, uint selector, uint ar);
};

constexpr uint AR_INTGATE32 =0x008e;
constexpr uint AR_TSS32 = 0x0089;

void SEGMENT_DESCRIPTOR::set(uint limit, uint base, uint ar)
{
	// limit =0xffffffff, base=0x00000000 ar = 0x409a
	if (limit > 0xfffff){
		ar |= 0x8000;	// G_bit = 1
		limit /= 0x1000;
	}
	limit_low = limit & 0xffff;
	base_low = base & 0xffff;
	base_mid = (base >> 16) & 0xff;
	access_right = ar & 0xff;
	limit_high = ((limit >> 16) & 0x0f) | ((ar>>8) & 0xf0);
	base_high = (base >> 24) & 0xff;
}

void GATE_DESCRIPTOR::set(uint offset, uint sel, uint ar)
{
	offset_low = offset & 0xffff;
	selector = sel;
	dw_count = (ar>>8) & 0xff;
	access_right = ar & 0xff;
	offset_high = (offset >> 16) & 0xffff;

}

#pragma pack(push, 1)
struct  OS_LOAD_GDT
{
	short limit;
	SEGMENT_DESCRIPTOR*  base;
	OS_LOAD_GDT() = delete;
	OS_LOAD_GDT(short limit, SEGMENT_DESCRIPTOR* base) : limit(limit), base(base)
	{
		auto* p = this;
		__asm {
			mov	 eax, p
			lgdt [eax]
		};
	}
};

struct OS_LOAD_IDT
{
	short limit;
	GATE_DESCRIPTOR*  base;
	OS_LOAD_IDT() = delete;
	OS_LOAD_IDT(short limit, GATE_DESCRIPTOR* base) : limit(limit), base(base)
	{
		auto* p = this;
		__asm {
			mov	 eax, p
			lidt [eax]
		}
	}
};

#pragma pack(pop) 


#endif

