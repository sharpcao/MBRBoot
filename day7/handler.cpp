#include <cstdio>
#include "handler.h"
#include "input_device.h"
#include "vga.h"
#include "os_io.h"
#include "CEventBuf.h"


CEventBuf<64> KeyBuf;
CEventBuf<128> MouseBuf;

void int21_handler()
{
    io_out8(PIC::PIC0_OCW2,0x61);
    KeyBuf.set_char(io_in8(PORT::PORT_KEYDAT));

}

void int2c_handler()
{
    io_out8(PIC::PIC1_OCW2,0x64);
    io_out8(PIC::PIC0_OCW2,0x62);
    MouseBuf.set_char(io_in8(PORT::PORT_KEYDAT));

}

void handle_message()
{
    CPalette pal;
    uchar c, s[20];
    for(;;){
        io_cli();
        
        if ( (c=KeyBuf.get_char()))
        {
            io_sti();
            sprintf((char*)s,"Key:%02X",c);
            pal.boxfill(Color8::COL8_008484,0,46,80,62);
            pal.xyprint(5,46,(char*)s);           
        }else if ( (c=MouseBuf.get_char()) )
        {        
            io_sti();
            sprintf((char*)s,"Mouse:%02X",c );
            pal.boxfill(Color8::COL8_008484,0,66,80,82);
            pal.xyprint(5,66,(char*)s );  
        }else
        {
            io_stihlt();
        }
    }
}