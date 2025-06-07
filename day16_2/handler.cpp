#include "inc\handler.h"
#include "inc\device.h"
#include "inc\vga.h"
#include "inc\os_io.h"
#include "inc\task_kit.h"
#include "inc\decode_kit.h"
#include "inc\layer.h"
#include "inc\winos.h"
#include "inc\timer_kit.h"


Task_Message_mgr EventList;


extern CWinOS OS;
extern char keytable[];
extern CTimerCtrl task_b_timectl;

void int20_handler()
{

    io_out8(PIC::PIC0_OCW2,0x60);
    OS.timer_ctrl.mt_inc();

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

void mouse_event(const Mouse_decode& md)
{
    CVGA vga;

    Layer_mgr& ly_mgr = *OS.p_layerMgr;
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
    static Mouse_decode mdec;

    OS._speedcnt = 0;
    for(;;){

        OS._speedcnt++; 
        io_cli();        
        if ( !EventList.get_message(p1,p2))
        {
            EventList.deact_msg_task();
            io_sti();
        }else{
            io_sti();
            if( p1 == EVENT::Timer){
                OS.timer_ctrl.call_hander((uint)p2);
            }
            else if(p1 == EVENT::Key){
                char s1[5];
                uint2str(s1,p2);
                OS.layers.p_txt_key->set_text(s1,*OS.p_layerMgr);
                OS.layers.p_input->add_key(p2,*OS.p_layerMgr);

            }else if (p1 == EVENT::Mouse){
                if(mdec.push_char((char)p2)){
                    mouse_event(mdec);
                }
            }
        }
    }
}




