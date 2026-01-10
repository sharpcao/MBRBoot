#include "inc\bootpack.h"
#include "inc\console.h"


CWinOS OS;


void tick_timeout(uint tmr);

void os_main(BOOTINFO *pbi)
{


    GDTIDT gdtidt;
    gdtidt.init_gdt_idt();      

    CMEM_MGR mem_mgr(0x300000,0x100000);
    OS.init(pbi, mem_mgr);


    uint mem_size = memtest(0x400000,0xbfffffff);
    stringbuf<> mem_max_str;
    mem_max_str <<"memory:" << mem_size/(1024*1024) <<" MB";

    uint width = OS.screen_width * 0.8;
    uint height = OS.screen_height * 0.7;
    uint offset_x = 30;
    uint offset_y = OS.screen_height*0.2;

    // Window* p_win = OS.CreateWindow<Window>(OS.screen_width * 0.65, 2, OS.screen_width * 0.35, OS.screen_height *0.45);
    // p_win->redraw();
    // p_win->xyprint(5,30,mem_max_str.c_str(),Color8::COL8_000000);

    ConsoleWindow* p_console = OS.CreateWindow<ConsoleWindow>(offset_x, offset_y, width,height);
    p_console->set_active();
    p_console->redraw();

    OS.p_Console = p_console;

    OS.p_layerMgr->refresh();

    OS.timer_ctrl.add_timer(100,tick_timeout);

    handle_message();

}

void tick_timeout(uint tmr)
{
    static uint count = 0;
    count++;
    stringbuf<> s;
    s << (uint)count << "s";
    auto pd = OS.layers.p_txt_tick;
    pd->set_text(s.c_str(),*OS.p_layerMgr,Color8::COL8_000000);
    OS.timer_ctrl.set_timer(tmr,100,tick_timeout);
}




void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
