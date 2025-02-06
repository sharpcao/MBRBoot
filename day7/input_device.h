#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

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


#endif