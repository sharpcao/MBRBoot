#ifndef DEVICE_H
#define DEVICE_H

using uint = unsigned int;

enum PORT{
	 PORT_KEYDAT = 0x0060,
	 PORT_KEYSTA =0x0064,
	 PORT_KEYCMD =0x0064,
	 KEYSTA_SEND_NOTREADY =0x02,
	 KEYCMD_WRITE_MODE =0x60,
	 KBC_MODE =0x47,
	 KEYCMD_SENDTO_MOUSE= 0xd4,
	 MOUSECMD_ENABLE= 0xf4
};

class CInput_Device
{
public:
	void init_keyboard();
	void enable_mouse();
private:
	void wait_KBC_sendready();

};

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
	void set_interrupt();
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