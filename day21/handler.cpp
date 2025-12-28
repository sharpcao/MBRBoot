#include "inc\handler.h"
#include "inc\device.h"
#include "inc\vga.h"
#include "inc\os_io.h"
#include "inc\task_kit.h"
#include "inc\decode_kit.h"
#include "inc\layer.h"
#include "inc\winos.h"
#include "inc\timer_kit.h"
#include "inc\winform.h"


Task_Message_mgr EventList;


extern CWinOS OS;
extern char keytable[];
extern CTimerCtrl task_b_timectl;

void get_reg_str(stringbuf<>& str)
{
    cpu::Reg reg;
    cpu::get_regs(&reg);
    str << stringbuf<>::flag::hex
        << "\neax:" << reg.eax 
        << ",ebx:" << reg.ebx
        << ",ecx:" << reg.ecx
        << ",edx:" << reg.edx
        << ",esi:" << reg.esi
        <<", edi:" << reg.edi
        <<"\ncs:" << reg.cs
        <<",ds:" << reg.ds
        <<",es:" << reg.es
        <<",fs:" << reg.fs
        <<"\nflag:" << reg.eflags 
        << stringbuf<>::flag::dec;
}

void get_task_str(uint idx, stringbuf<>& str)
{
    struct SEG{
        uint low;
        uint high;
    };

    TaskItem* task = OS.p_task_mgr->get_task(idx);
    TSS32& tss = task->tss;

    GDTIDT gdt;
    //SEG* pseg = (SEG*)gdt.get_gdt(task->sel);
    SEG* pseg = (SEG*)gdt.get_gdt(0);

    str <<"\n" << idx << stringbuf<>::flag::hex
        << ",sel:" << task->sel
        << "\n,ss:" <<(uint)tss.ss
        << ",esp:" <<  (uint)tss.esp
        << ",eip:" <<(uint)tss.eip
        << ",efg:" <<(uint)tss.eflags
        << "\ncs:" <<(uint)tss.cs 
        << ", ds:" <<(uint)tss.ds
        << ", es:" <<(uint)tss.es
        << ", fs:" <<(uint)tss.fs
        << ", gs:" <<(uint)tss.gs
        << ", ss:" <<(uint)tss.ss
        << "\n,seg1_0:" << pseg[1].low << ",seg1_1:" <<pseg[1].high
        << "\n,seg2_0:" << pseg[2].low << ",seg2_1:" <<pseg[2].high
        << "\n,seg3_0:" << pseg[3].low << ",seg3_1:" <<pseg[3].high
        << "\n,seg4_0:" << pseg[4].low << ",seg4_1:" <<pseg[4].high
        << "\n,seg6_0:" << pseg[6].low << ",seg6_1:" <<pseg[6].high;

}

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

void int7_handler()
{
    uint cr0 = load_cr0();
    cr0 &= ~(0x08);   //clear TS
    store_cr0(cr0);
   
}

void int0a_handler(uint int_num)
{
    uint _eip;
    __asm{ 
        mov eax,_eip
        mov _eip, eax 
    }
    CVGA& vga = OS.vga;
    vga.fill(0,0, vga.get_xsize(), vga.get_ysize(),Color8::COL8_0000FF);
   
    stringbuf<> str;
    str << "int:" <<  stringbuf<>::flag::hex << int_num << ", 0x" << _eip ;
    get_task_str(0, str); 
    //get_task_str(1,str);
    //get_task_str(2,str);
    //get_task_str(3,str);  
    vga.xyprint(0,0,str.c_str(),Color8::COL8_FFFFFF);


    __asm{
    here:
        hlt
        jmp here 
    }
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

    for(;;){

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
                char s1[4];
                uint2str(s1,p2);
                OS.layers.p_txt_key->set_text(s1,*OS.p_layerMgr);
                OS.layers.p_input->add_key(p2,*OS.p_layerMgr);

                auto act_w = Window::get_active();
                if(act_w) {
                    
                    act_w->push_message(p1,p2);
            
                }


            }else if (p1 == EVENT::Mouse){
                if(mdec.push_char((char)p2)){
                    mouse_event(mdec);
                }
            }
        }
    }
}




