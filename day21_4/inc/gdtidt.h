#ifndef GDTIDT_H
#define GDTIDT_H

#include "task_kit.h"
#include "os_io.h"

using uint = unsigned int;


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



#pragma pack(push, 1)
struct  OS_LOAD_GDT
{
	short limit;
	SEGMENT_DESCRIPTOR*  base;
	OS_LOAD_GDT() = delete;
	OS_LOAD_GDT(short limit, SEGMENT_DESCRIPTOR* base) : limit(limit), base(base)
	{
		load_gdt(this);
	}
};

struct OS_LOAD_IDT
{
	short limit;
	GATE_DESCRIPTOR*  base;
	OS_LOAD_IDT() = delete;
	OS_LOAD_IDT(short limit, GATE_DESCRIPTOR* base) : limit(limit), base(base)
	{
		load_idt(this);
	}
};

#pragma pack(pop) 


struct TSS32;

class  GDTIDT{
public:
	using Handler_Func = void(*)();
	void init_gdt_idt();
	void add_gdt_task(uint idx, TSS32*);
	void add_idt_handler(uint id, Handler_Func f, bool dpl_0 = true );
	SEGMENT_DESCRIPTOR* get_gdt(uint sel){
		return _gdt_start + sel;
	}
	SEGMENT_DESCRIPTOR* operator[] (uint sel){
		return _gdt_start + sel;
	}

private:
	void _load_gdt() { OS_LOAD_GDT gdt(_gdt_size, _gdt_start);}
	void _load_idt() { OS_LOAD_IDT idt(_idt_size, _idt_start);}

	static short _gdt_size;	
	static SEGMENT_DESCRIPTOR* _gdt_start;
	static short _idt_size;
	static GATE_DESCRIPTOR* _idt_start;

};

#define AR_CODE32_ER 0x409a
#define AR_DATA32_RW 0x4092


#endif