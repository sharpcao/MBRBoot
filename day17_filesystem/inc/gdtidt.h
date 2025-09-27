#ifndef GDTIDT_H
#define GDTIDT_H

#include "task_kit.h"

using uint = unsigned int;
constexpr uint AR_INTGATE32 =0x008e;
constexpr uint AR_TSS32 = 0x0089;

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


struct TSS32;

class  GDTIDT{
public:
	using Handler_Func = void(*)();
	void init_gdt_idt();
	void add_gdt_task(uint idx, TSS32*);


	void add_idt_handler(uint id, Handler_Func f);

private:
	void _load_gdt() { OS_LOAD_GDT gdt(_gdt_size, _gdt_start);}
	void _load_idt() { OS_LOAD_IDT idt(_idt_size, _idt_start);}

	static short _gdt_size;	
	static SEGMENT_DESCRIPTOR* _gdt_start;
	static short _idt_size;
	static GATE_DESCRIPTOR* _idt_start;

};




#endif