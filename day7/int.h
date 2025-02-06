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





template<uint N>
class CEventBuf
{

public:
	
	bool set_char(char c);
	char get_char();
	bool is_empty() { return (_i1 == _i2);}

private:
	uint _i1=0, _i2 = 0;
	char _data[N] ={0};
	
};

template<uint N>
bool CEventBuf<N>::set_char(char c)
{
	if ( (_i2 + 1 ) % N == _i1 ){
		return false;
	}else{
		_data[_i2++] = c;
		if (_i2 == N) {_i2 = 0;}
		return true;
	}
}

template<uint N>
char CEventBuf<N>::get_char()
{
	char c = 0;
	if (_i1 != _i2){
		c = _data[_i1++];
		if(_i1 == N) {_i1 = 0;}
		if(_i1 == _i2) {_i1 = _i2 = 0;}
	}
	return c;
}



#endif