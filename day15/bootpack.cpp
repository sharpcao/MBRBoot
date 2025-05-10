#include "inc\bootpack.h"


CWinOS OS;
TSS32 tss_a, tss_b;


void init_tss_b(uint task_b_esp);
void tick_timeout(uint tmr);
void tt_timeout(uint tmr);
void timer5_timeout(uint tmr);
void timer10_timeout(uint tmr);
void task_b_main(void);


void os_main(BOOTINFO *pbi)
{

    OS.vga.init(pbi);

    CMEM_MGR mem_mgr(0x200000,0x100000);

    OS.init_layers(mem_mgr, pbi->scrnx,pbi->scrny);
    OS.init_timers();
   

    uint mem_size = memtest(0x400000,0xbfffffff);
    stringbuf<> mem_max_str;
    mem_max_str <<"memory:" << mem_size/(1024*1024) <<" MB";
    OS.layers.p_window1->xyprint(5,30,mem_max_str.c_str(),Color8::COL8_000000);
    OS.p_layerMgr->refresh();

  

    GDTIDT gdtidt;
    gdtidt.init_gdt_idt();  


    init_tss_b(mem_mgr.malloc(1024) + 1024);
    gdtidt.add_gdt_task(4, &tss_a);
    gdtidt.add_gdt_task(5, &tss_b);
    

    gdtidt.add_idt_handler(0x20, handler_wrap<int20_handler>);
    gdtidt.add_idt_handler(0x21, handler_wrap<int21_handler>);
    gdtidt.add_idt_handler(0x2c, handler_wrap<int2c_handler>);
    gdtidt.load_gdt();
    gdtidt.load_idt();
    io_load_tr(4*8);

   
    
    CPIC pic;
    pic.init_pic();
    pic.set_interrupt();

    OS.timer_ctrl.add_timer(100,tick_timeout);
    OS.timer_ctrl.add_timer(500, timer5_timeout);
    OS.timer_ctrl.add_timer(2,tt_timeout);
    
    CPIT pit;
    pit.init_pit();

    CInput_Device dev;
    dev.init_keyboard();
    dev.enable_mouse();
    pic.enable_key_mouse();

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
void tt_timeout(uint tmr)
{
    task_switch((uint)task_b_main,5*8);
    OS.timer_ctrl.set_timer(tmr,2,tt_timeout);
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


CEventBuf<> task_b_event_list;
CTimerCtrl task_b_timectl;
void task_b_tt_timeout(uint tmr)
{
    task_switch(0,4*8);
    task_b_timectl.set_timer(tmr,2,task_b_tt_timeout);
}

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
    task_b_timectl.add_timer(2, task_b_tt_timeout);
    task_b_timectl.add_timer(100,task_b_timer1_timeout);

    uint p1,p2;
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

void init_tss_b(uint task_b_esp)
{
    tss_a.ldtr = 0;
    tss_a.iomap = 0x40000000;
    tss_b.ldtr = 0;
    tss_b.iomap = 0x40000000;

    tss_b.eip = (int) &task_b_main;
    tss_b.eflags = 0x00000202;
    tss_b.eax = 0;
    tss_b.ecx = 0;
    tss_b.edx = 0;
    tss_b.ebx = 0;
    tss_b.esp = task_b_esp;
    tss_b.ebp = 0;
    tss_b.esi = 0;
    tss_b.edi = 0;
    tss_b.es = 2*8;
    tss_b.cs = 1*8;
    tss_b.ss = 3*8;
    tss_b.ds = 2*8;
    tss_b.fs = 2*8;
    tss_b.gs = 2*8;

}

void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
