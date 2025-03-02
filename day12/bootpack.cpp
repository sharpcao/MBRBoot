#include "inc\bootpack.h"


C_WOS OS;


void os_main(BOOTINFO *pbi)
{

    CVGA vga(pbi);
    vga.init_palette();

    CMEM_MGR mem_mgr(0x200000,0x100000);

    init_layers(mem_mgr, pbi->scrnx,pbi->scrny);
    OS.init();
   

    uint mem_size = memtest(0x400000,0xbfffffff);
    stringbuf<> mem_max_str;
    mem_max_str <<"memory:" << mem_size/(1024*1024) <<" MB";
    OS.layers.p_window1->xyprint(5,30,mem_max_str.c_str(),Color8::COL8_000000);

    vga.map(OS.p_layerMgr->refresh());  

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

    OS.timer_ctrl.add_timer(100,timer0_timeout);
    OS.timer_ctrl.add_timer(1000,timer1_timeout);
    
    CPIT pit;
    pit.init_pit();

    CInput_Device dev;
    dev.init_keyboard();
    dev.enable_mouse();
    pic.enable_key_mouse();

    handle_message();

}

void timer0_timeout()
{
    static uint count = 0;
    count++;
    stringbuf<10> s;
    s << (uint)count << " s";
    // char s[10];
    // uint2str(s,++count);
    OS.debug_print(s.c_str());
    OS.timer_ctrl.set_timer(0,100,timer0_timeout);
}
void timer1_timeout()
{
    static CVGA vga;
    auto p_timer = OS.layers.p_txt_timer;
    p_timer->set_text("Timer1 out",Color8::COL8_FFFF00);
    vga.map((*OS.p_layerMgr)[0]->get_mem(),p_timer->get_mem(),p_timer->get_area());
}

void init_layers(CMEM_MGR& mem_mgr, uint width, uint height)
{
    
    OS.p_Cursor = (CCursor*) mem_mgr.malloc(sizeof(CCursor));
    new(OS.p_Cursor) CCursor(width / 2, height/2,width,height);

    OS.p_layerMgr = (CLayer_Mgr*)mem_mgr.malloc(sizeof(CLayer_Mgr));
    new(OS.p_layerMgr) CLayer_Mgr(mem_mgr, width, height);

    CLayer_Mgr& ly_mgr = *OS.p_layerMgr;

    OS.layers.p_desktop =  (CDesktopLayer*)ly_mgr.add_layer(CDesktopLayer(width,height));
    OS.layers.p_desktop->load_img();
    OS.p_layerMgr->update(0,0,200,200);

    OS.p_layerMgr->set_bottom_idx(2);

    OS.layers.p_txt_hello = (CTextLayer* )ly_mgr.add_layer(CTextLayer(150,30,0,0));
    OS.layers.p_txt_hello->set_text("Hello OS!");

    OS.layers.p_txt_counter = (CTextLayer* )ly_mgr.add_layer(CTextLayer(100,20,0,20));
    OS.layers.p_txt_counter->set_text("Count");
    OS.layers.p_txt_counter->hide();
    
    OS.layers.p_txt_timer = (CTextLayer* )ly_mgr.add_layer(CTextLayer(120,20,0,40));
    OS.layers.p_txt_timer->set_text("Timer");
    OS.layers.p_txt_timer->hide();

    OS.layers.p_debug = (CTextLayer* )ly_mgr.add_layer(CTextLayer(120,20,0,100));
    OS.layers.p_debug->set_text("DEBUG",Color8::COL8_FF0000);
    OS.layers.p_debug->hide();




    OS.layers.p_window1 = (CWindowLayer*)ly_mgr.add_layer(CWindowLayer(150,100,100,40));
    OS.layers.p_window1->load_img("Window");
    
}

void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
