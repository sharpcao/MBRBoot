#include "inc\bootpack.h"


CWinOS OS;


void tick_timeout(uint tmr);
void timer5_timeout(uint tmr);
void timer10_timeout(uint tmr);


void task_b_main(uint);
void task_b_timer1_timeout(uint tmr);

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

    CWindowLayer* p_win_b[3];
    CTextLayer* p_text_b[3];
    uint pa[] = {2,6,1};


    for(uint i = 0; i < 3; ++i){
        uint width = OS.screen_width * 0.32;
        uint height = OS.screen_height * 0.38;
        uint offset_x = (width + 5) * i;
        uint offset_y = OS.screen_height * 0.36;
        p_win_b[i] = (CWindowLayer*)OS.p_layerMgr->create_layer(
                                                 CWindowLayer(offset_x,offset_y,width,height));
        stringbuf<> s("win_b_");
        s << pa[i];
        p_win_b[i]->load_img(s.c_str());

        p_text_b[i] = (CTextLayer*)OS.p_layerMgr->create_layer(
                                                CTextLayer(offset_x,offset_y+25,width,height -25));
        p_text_b[i]->set_text("text_b",Color8::COL8_000000);

    }


    OS.p_layerMgr->refresh();


    OS.timer_ctrl.add_timer(100,tick_timeout);
    OS.timer_ctrl.add_timer(500, timer5_timeout);

    /*

    for(uint i = 0; i<3;++i){
        Task* pt = OS.p_task_mgr->add_task(task_b_main, mem_mgr.malloc(8*1024) + 8*1024,(uint)p_text_b[i]); 
    
        if(i == 0){
            OS.p_task_mgr->set_active(pt,pa[i],2);
        }else{
            OS.p_task_mgr->set_active(pt,pa[i],3);
        }

    }
    */

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

void timer5_timeout(uint tmr)
{
    auto p_timer = OS.layers.p_txt_timer;
    p_timer->set_text("@5s Start", *OS.p_layerMgr, Color8::COL8_FFFF00);
    OS._speedcnt = 0 ;
    //OS.timer_ctrl.add_timer(1000,timer10_timeout);
    OS.timer_ctrl.add_timer(500,timer10_timeout);
    OS.p_task_mgr->set_active(OS.p_task_mgr->get_task(1),0,3);
    OS.p_task_mgr->set_active(OS.p_task_mgr->get_task(2),0,2);


}



void timer10_timeout(uint tmr)
{
    auto p_timer = OS.layers.p_txt_timer;
    auto p_win = OS.layers.p_window1;
    //auto p_speed = OS.layers.p_txt_speedcnt;

    stringbuf<> s;
    s << "c:" << OS._speedcnt;
    p_win->xyprint(5,50,s.c_str(),Color8::COL8_000000);
    OS.p_layerMgr->update(p_win->get_area());
    p_timer->set_text("10s passed", *OS.p_layerMgr, Color8::COL8_FFFF00);
    

}





void task_b_timer1_timeout(uint tmr)
{
    // stringbuf<> s;
    // s << "c:" << OS._speedcnt_task2;
    // auto p_txt = OS.layers.p_txt_task2;
    // p_txt->set_text(s.c_str(),*OS.p_layerMgr,Color8::COL8_FFFF00);
    //task_b_timectl.set_timer(tmr,100, task_b_timer1_timeout);
}

//CTimerCtrl task_b_timectl;
void task_b_main(uint param)
{

    auto ptext = (CTextLayer*)param;
    Task_Message_mgr task_b_event_list;
    OS.timer_ctrl.add_timer(100,task_b_timer1_timeout,&task_b_event_list);

    
    uint p1,p2;
    

    for(uint speedcnt_b = 0;; ++speedcnt_b){
        io_cli();
        if(!task_b_event_list.get_message(p1,p2)){
            io_sti();

        }else{
            io_sti();
            if(p1 == EVENT::Timer){
                OS.timer_ctrl.call_hander((uint)p2);
                stringbuf<> s;
                auto mgr = OS.p_task_mgr;
                s << mgr->get_cur_level() << ":" << mgr->get_cur_priority() << ":" << speedcnt_b / 10000;
 
                ptext->set_text(s.c_str(),*OS.p_layerMgr,Color8::COL8_000000);
          
                OS.timer_ctrl.set_timer(p2,100, task_b_timer1_timeout);

            }
        }
    }
}


void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
