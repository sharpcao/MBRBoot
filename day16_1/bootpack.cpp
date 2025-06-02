#include "inc\bootpack.h"


CWinOS OS;


void tick_timeout(uint tmr);
void timer5_timeout(uint tmr);
void timer10_timeout(uint tmr);


void task_b_main(void);
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
    OS.p_layerMgr->refresh();

    OS.timer_ctrl.add_timer(100,tick_timeout);
    OS.timer_ctrl.add_timer(500, timer5_timeout);

    Task* pt = OS.p_task_mgr->add_task(task_b_main, mem_mgr.malloc(1024) + 1024 - 8);  
    OS.p_task_mgr->set_active(pt);


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


Message_mgr<> task_b_event_list;
CTimerCtrl task_b_timectl;


void task_b_timer1_timeout(uint tmr)
{
    stringbuf<> s;
    s << "c:" << OS._speedcnt_task2;
    auto p_txt = OS.layers.p_txt_task2;
    p_txt->set_text(s.c_str(),*OS.p_layerMgr,Color8::COL8_FFFF00);
    task_b_timectl.set_timer(tmr,100, task_b_timer1_timeout);
}

void task_b_main(void)
{
    task_b_timectl.init(&task_b_event_list);
    task_b_timectl.add_timer(100,task_b_timer1_timeout);
    //task_b_timectl.add_timer(2, task_b_tt_timeout);
    
    uint p1,p2;
    OS._speedcnt_task2 = 0;

    for(;;){
        OS._speedcnt_task2++;
        io_cli();
        if(!task_b_event_list.get_message(p1,p2)){
            io_sti();

        }else{
            io_sti();
            if(p1 == EVENT::Timer){
                task_b_timectl.call_hander((uint)p2);
            }
        }
    }
}


void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
