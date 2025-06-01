#include "inc\gdtidt.h"

short GDTIDT::_gdt_size = 0x10000-1;
SEGMENT_DESCRIPTOR* GDTIDT::_gdt_start = (SEGMENT_DESCRIPTOR*)0x00270000;
short GDTIDT::_idt_size = 0x800-1;
GATE_DESCRIPTOR* GDTIDT::_idt_start = (GATE_DESCRIPTOR*)0x0026f800;


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

void GDTIDT::init_gdt_idt()
{
	
	for (auto it = _gdt_start; it <= _gdt_start + _gdt_size; ++it)
	{
		it->set(0,0,0);
	}
	for (auto it = _idt_start; it <= _idt_start+_idt_size; ++it)
	{
		//it->set((int)0x7c00,1*8,AR_INTGATE32);
		it->set(0,0,0);
	}

	(_gdt_start+1)->set(0xffffffff, 0x00000000, 0x409a); //code segment
	(_gdt_start+2)->set(0xffffffff, 0x00000000, 0x4092); //data segment
	(_gdt_start+3)->set(0x00000000, 0x00000000, 0x4096); //stack segment

	_load_gdt();
	_load_idt();
}

void GDTIDT::add_gdt_task(uint idx, TSS32* p_tss)
{
	(_gdt_start+idx)->set(sizeof(TSS32)-1, (uint)p_tss, AR_TSS32);
}


void GDTIDT::add_idt_handler(uint id, Handler_Func f)
{
	(_idt_start+id)->set((int)f,1*8,AR_INTGATE32);
}