#ifndef INT_H
#define INT_H

#include "os_io.h"
using uint = unsigned int;

class CPIC{
public:
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
	void init_pic();	
	void set_interrupt() {io_sti();}
	void enable_key_mouse();
};

class CPIT{
public:
	enum PIT{
		PIT_CTRL = 0x0043,
		PIT_CNT0 = 0x0040
	};
	static void init_pit();

};





#endif