#include "inc\handler.h"
#include "inc\input_device.h"
#include "inc\vga.h"
#include "inc\os_io.h"
#include "inc\CEventBuf.h"
#include "inc\layer.h"



CEventBuf<64> KeyBuf;
CEventBuf<128> MouseBuf;

extern C_WOS OS;

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

void mouse_event(const CMouseDecode& md)
{
    CVGA vga;
    //static CCursor cursor(vga.get_xsize()/2 ,vga.get_ysize()/2, vga.get_xsize(),vga.get_ysize());
    CLayer_Mgr& ly_mgr = *OS.p_layerMgr;
    CCursor& cursor = *OS.p_Cursor;

    if((md.dx !=0 )|| (md.dy != 0)) {
        CRect rt0(cursor._x, cursor._y,CURSOR_WIDTH,CURSOR_HEIGHT);
        cursor.move_cursor_by(md.dx, md.dy);
        CRect rt1(cursor._x, cursor._y,CURSOR_WIDTH,CURSOR_HEIGHT);
        vga.map(ly_mgr[0]->get_mem(),rt0);
        vga.map(ly_mgr[0]->get_mem(),cursor.get_image(),rt1);
    }

}

void handle_message()
{
    CVGA vga;
    uchar c;
    static CMouseDecode mdec;
    CLayer_Mgr& ly_mgr = *OS.p_layerMgr;
    uint count = 0;
    //stringbuf<> s;
    char s[10];

    CTextLayer* p_count = (CTextLayer* )ly_mgr.add_layer(CTextLayer(50,20,0,20));
    p_count->set_text("Count");
    p_count->hide();
    ly_mgr.update(p_count->get_area());
    
    for(;;){

        uint2str(s,++count);
         p_count->set_text(s,Color8::COL8_FFFF00);
        // ly_mgr.update(p_count->get_area());
         //vga.map(ly_mgr[0]->get_mem(),p_count->get_area());
        vga.map(ly_mgr[0]->get_mem(),p_count->get_mem(),p_count->get_area());

        io_cli();
        
        if ( KeyBuf.get_char((char*)&c))
        {
            io_sti();
        
        }else if ( MouseBuf.get_char((char*)&c))
        {        
            io_sti();           
            if (mdec.push_char(c)){
                mouse_event(mdec);           
            }
        }else
        {
            //io_stihlt();
            io_sti();
        }
    }
}