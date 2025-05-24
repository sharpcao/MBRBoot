#include "inc\int.h"
#include "inc\os_io.h"


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
	io_out8(PIC::PIC0_IMR, 0xf8); /* PIT和PIC1和键盘设置为许可(11111000) */
    io_out8(PIC::PIC1_IMR, 0xef); /* 鼠标设置为许可(11101111) */
}


void CPIT::init_pit()
{
	static bool is_init = false;
	if (!is_init) {
		io_out8(PIT::PIT_CTRL,0x34);
		io_out8(PIT::PIT_CNT0, 0x9c);
		io_out8(PIT::PIT_CNT0,0x2e);
		//set 100Hz, 11932 = 0x2e9c 
	}
	is_init = true;
}



