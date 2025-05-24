#include "inc\os_io.h"
#include "inc\input_device.h"

void CInput_Device::wait_KBC_sendready()
{
	for(;;)
	{
		if((io_in8(PORT::PORT_KEYSTA) & PORT::KEYSTA_SEND_NOTREADY)==0){
			break;
		}
	}

}

void CInput_Device::init_keyboard()
{
	wait_KBC_sendready();
	io_out8(PORT::PORT_KEYCMD,PORT::KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT::PORT_KEYDAT,PORT::KBC_MODE);
}

void CInput_Device::enable_mouse()
{
	wait_KBC_sendready();
	io_out8(PORT::PORT_KEYCMD, PORT::KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT::PORT_KEYDAT, PORT::MOUSECMD_ENABLE);
}