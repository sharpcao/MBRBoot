#include "inc\bootpack.h"


CWinOS OS;
void tick_timeout(uint tmr);
void timer5_timeout(uint tmr);
void timer10_timeout(uint tmr);


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
    gdtidt.add_idt_handler(0x20, handler_wrap<int20_handler>);
    gdtidt.add_idt_handler(0x21, handler_wrap<int21_handler>);
    gdtidt.add_idt_handler(0x2c, handler_wrap<int2c_handler>);
    gdtidt.load_gdt();
    gdtidt.load_idt();

    CPIC pic;
    pic.init_pic();
    pic.set_interrupt();

    OS.timer_ctrl.add_timer(100,tick_timeout);
    OS.timer_ctrl.add_timer(500, timer5_timeout);
    
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
    OS.timer_ctrl.add_timer(1000,timer10_timeout);

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


void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
