#include "inc\bootpack.h"
#include "inc\console.h"


CWinOS OS;


void tick_timeout(uint tmr);


//void task_b_main(uint);


void os_main(BOOTINFO *pbi)
{


    GDTIDT gdtidt;
    gdtidt.init_gdt_idt();      

    CMEM_MGR mem_mgr(0x200000,0x100000);
    OS.init(pbi, mem_mgr);


    uint mem_size = memtest(0x400000,0xbfffffff);
    stringbuf<> mem_max_str;
    mem_max_str <<"memory:" << mem_size/(1024*1024) <<" MB";
    OS.layers.p_window1->xyprint(5,30,mem_max_str.c_str(),Color8::COL8_000000);

   
  
    uint width = OS.screen_width * 0.6;
    uint height = OS.screen_height * 0.7;
    uint offset_x = 30;
    uint offset_y = OS.screen_height*0.2;
    ConsoleWindow* p_console = ConsoleWindow::CreateWindow(offset_x, offset_y, width,height);

    

    OS.p_layerMgr->refresh();


    OS.timer_ctrl.add_timer(100,tick_timeout);

    handle_message();

}

void tick_timeout(uint tmr)
{
    static uint count = 0;
    count++;
    stringbuf<10> s;
    s << (uint)count << "s";
    auto pd = OS.layers.p_txt_tick;
    pd->set_text(s.c_str(),*OS.p_layerMgr,Color8::COL8_000000);
    OS.timer_ctrl.set_timer(tmr,100,tick_timeout);
}





//CTimerCtrl task_b_timectl;
// void task_b_main(uint param)
// {

//     auto ptext = (CTextLayer*)param;
//     Task_Message_mgr task_b_event_list;
//     OS.timer_ctrl.add_timer(100,task_b_timer1_timeout,&task_b_event_list);

    
//     uint p1,p2;
    

//     for(uint speedcnt_b = 0;; ++speedcnt_b){
//         io_cli();
//         if(!task_b_event_list.get_message(p1,p2)){
//             io_sti();

//         }else{
//             io_sti();
//             if(p1 == EVENT::Timer){
//                 OS.timer_ctrl.call_hander((uint)p2);
//                 stringbuf<> s;
//                 auto mgr = OS.p_task_mgr;
//                 s << mgr->get_cur_level() << ":" << mgr->get_cur_priority() << ":" << speedcnt_b / 10000;
 
//                 ptext->set_text(s.c_str(),*OS.p_layerMgr,Color8::COL8_000000);
          
//                 OS.timer_ctrl.set_timer(p2,100, task_b_timer1_timeout);

//             }
//         }
//     }
// }


void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
