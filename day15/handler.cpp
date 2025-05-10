#include "inc\handler.h"
#include "inc\input_device.h"
#include "inc\vga.h"
#include "inc\os_io.h"
#include "inc\CEventBuf.h"
#include "inc\layer.h"
#include "inc\global.h"
#include "inc\timer.h"


CEventBuf<> EventList;


extern CWinOS OS;
extern char keytable[];
extern CTimerCtrl task_b_timectl;

void int20_handler()
{

    io_out8(PIC::PIC0_OCW2,0x60);
    OS.timer_ctrl.inc();
    task_b_timectl.inc();

}
void int21_handler()
{
    io_out8(PIC::PIC0_OCW2,0x61);

    EventList.push_message(EVENT::Key, (uint)io_in8(PORT::PORT_KEYDAT));


}

void int2c_handler()
{
    io_out8(PIC::PIC1_OCW2,0x64);
    io_out8(PIC::PIC0_OCW2,0x62);

    EventList.push_message(EVENT::Mouse, (uint)io_in8(PORT::PORT_KEYDAT));

}

void mouse_event(const CMouseDecode& md)
{
    CVGA vga;

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
   
    uint p1,p2;
    static CMouseDecode mdec;

    OS._speedcnt = 0;
    for(;;){

        OS._speedcnt++;
  
        io_cli();        
        if ( !EventList.get_message(p1,p2))
        {
            //io_stihlt();
            io_sti();
            continue;
        }else{
            io_sti();
            if( p1 == EVENT::Timer){
                OS.timer_ctrl.call_hander((uint)p2);
            }else if(p1 == EVENT::Key){
                char s1[5];
                uint2str(s1,p2);
                OS.layers.p_txt_key->set_text(s1,*OS.p_layerMgr);
                // if(p2 < 0x54) {
                //     stringbuf<> skey ;
                //     skey << keytable[p2];
                //     OS.layers.p_txt_hello->set_text(skey.c_str(),*OS.p_layerMgr);
                // }
                OS.layers.p_input->add_key(p2,*OS.p_layerMgr );

            }else if (p1 == EVENT::Mouse){
                if(mdec.push_char((char)p2)){
                    mouse_event(mdec);
                }
            }
        }
    }
}

