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
    static CMouseDecode mdec;
    for(;;){
        io_cli();
        
        if ( KeyBuf.get_char((char*)&c))
        {
            io_sti();
            sprintf((char*)s,"Key:%02X",c);
            pal.boxfill(Color8::COL8_008484,0,46,120,62);
            pal.xyprint(5,46,(char*)s);           
        }else if ( MouseBuf.get_char((char*)&c))
        {        
            io_sti();
            sprintf((char*)s,"M:%02X",c);
            pal.boxfill(Color8::COL8_008484,0,66,200,86);
            pal.xyprint(5,66,(char*)s); 
            
            if (mdec.push_char(c)){
                sprintf((char*)s, "[lcr %4d %4d]", mdec.dx, mdec.dy);
                if ((mdec.btn & 0x01) != 0) {
                    s[1] = 'L';
                }
                if ((mdec.btn & 0x02) != 0) {
                    s[3] = 'R';
                }
                if ((mdec.btn & 0x04) != 0) {
                    s[2] = 'C';
                }
                pal.hide_cursor();
                pal.move_cursor_by(mdec.dx,mdec.dy);
                pal.draw_cursor();

                pal.xyprint(50,66,(char*)s );  
            }
        }else
        {
            io_stihlt();
        }
    }
}