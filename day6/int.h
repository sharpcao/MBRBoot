#ifndef INT_H
#define INT_H

#include "os_io.h"
using uint = unsigned int;

class  GDTIDT{
public:
	using Handler_Func = void(*)();
	void init_gdt_idt();
	void load_gdt() { OS_LOAD_GDT gdt(_gdt_size, _gdt_start);}
	void load_idt() { OS_LOAD_IDT idt(_idt_size, _idt_start);}
	void add_idt_handler(uint id, Handler_Func f);
	void init_pic();
private:

	static short _gdt_size;	
	static SEGMENT_DESCRIPTOR* _gdt_start;
	static short _idt_size;
	static GATE_DESCRIPTOR* _idt_start;

};


short GDTIDT::_gdt_size = 0x10000-1;
SEGMENT_DESCRIPTOR* GDTIDT::_gdt_start = (SEGMENT_DESCRIPTOR*)0x00270000;
short GDTIDT::_idt_size = 0x800-1;
GATE_DESCRIPTOR* GDTIDT::_idt_start = (GATE_DESCRIPTOR*)0x0026f800;


#endif