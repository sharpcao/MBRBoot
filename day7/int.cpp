#include "int.h"
#include "os_io.h"


void CPIC::init_pic()
{
	io_out8(PIC::PIC0_IMR, 0xff ); /* 禁止所有中断 */
	io_out8(PIC::PIC1_IMR, 0xff ); /* 禁止所有中断 */
	io_out8(PIC::PIC0_ICW1, 0x11 ); /* 边沿触发模式（edge trigger mode） */
	io_out8(PIC::PIC0_ICW2, 0x20 ); /* IRQ0-7由INT20-27接收 */
	io_out8(PIC::PIC0_ICW3, 1 << 2); /* PIC1由IRQ2连接 */
	io_out8(PIC::PIC0_ICW4, 0x01 ); /* 无缓冲区模式 */
	io_out8(PIC::PIC1_ICW1, 0x11 ); /* 边沿触发模式（edge trigger mode） */
	io_out8(PIC::PIC1_ICW2, 0x28 ); /* IRQ8-15由INT28-2f接收 */
	io_out8(PIC::PIC1_ICW3, 2 ); /* PIC1由IRQ2连接 */
	io_out8(PIC::PIC1_ICW4, 0x01 ); /* 无缓冲区模式 */
	io_out8(PIC::PIC0_IMR, 0xfb ); /* 11111011 PIC1以外全部禁止 */
	io_out8(PIC::PIC1_IMR, 0xff ); /* 11111111 禁止所有中断 */
}
void CPIC::enable_key_mouse()
{
	io_out8(PIC::PIC0_IMR, 0xf9); 
    io_out8(PIC::PIC1_IMR, 0xef); 
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

	(_gdt_start+1)->set(0xffffffff, 0x00000000, 0x409a);
	(_gdt_start+2)->set(0xffffffff, 0x00000000, 0x4092);
	(_gdt_start+3)->set(0x00000000, 0x00000000, 0x4096);

	
}

void GDTIDT::add_idt_handler(uint id, Handler_Func f)
{
	(_idt_start+id)->set((int)f,1*8,AR_INTGATE32);
}

