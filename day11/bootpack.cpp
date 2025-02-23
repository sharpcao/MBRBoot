#include "inc\bootpack.h"

C_WOS OS;

void os_main(BOOTINFO *pbi)
{

    CVGA vga(pbi);
    vga.init_palette();

    CMEM_MGR mem_mgr(0x200000,0x100000);
    OS.p_layerMgr = (CLayer_Mgr*)mem_mgr.malloc(sizeof(CLayer_Mgr));
    new(OS.p_layerMgr) CLayer_Mgr(mem_mgr,pbi->scrnx,pbi->scrny);
    CLayer_Mgr& ly_mgr = *OS.p_layerMgr;

    OS.p_Cursor = (CCursor*) mem_mgr.malloc(sizeof(CCursor));
    new(OS.p_Cursor) CCursor(vga.get_xsize()/2 ,vga.get_ysize()/2, vga.get_xsize(),vga.get_ysize());


    CDesktopLayer* p_desktop = (CDesktopLayer*)ly_mgr.add_layer(CDesktopLayer(pbi->scrnx,pbi->scrny));
    p_desktop->load_img();  
    vga.map(ly_mgr.refresh());
    ly_mgr.set_bottom_idx(2);

    uint mem_size = memtest(0x400000,0xbfffffff);
    stringbuf<> mem_max_str;
    mem_max_str <<"memory:" << mem_size/(1024*1024) <<" MB";
    //vga.xyprint(5,5,s.c_str());

    CTextLayer* p_text = (CTextLayer* )ly_mgr.add_layer(CTextLayer(150,30,0,0));

    p_text->set_text("Hello OS!");
    vga.map(ly_mgr.update(p_text->get_area()),p_text->get_area());

    // auto p_rect = ly_mgr.add_layer(CLayer(150,30,20,20));
    // p_rect->load_img(Color8::COL8_840000);
    // //p_rect->hide();
    // //ly_mgr.set_to_top(*p_text);
    // ly_mgr.set_to_bottom(*p_rect);
    // vga.map(ly_mgr.update(p_rect->get_area()),p_text->get_area());
    

    CWindowLayer* p_window = (CWindowLayer*)ly_mgr.add_layer(CWindowLayer(150,100,100,40));
    p_window->load_img("Window");
    p_window->xyprint(5,30,mem_max_str.c_str(),Color8::COL8_000000);
    vga.map(ly_mgr.update(p_window->get_area()),p_window->get_area());
   



    GDTIDT gdtidt;
    gdtidt.init_gdt_idt();  
    gdtidt.add_idt_handler(0x21, handler_wrap<int21_handler>);
    gdtidt.add_idt_handler(0x2c, handler_wrap<int2c_handler>);
    gdtidt.load_gdt();
    gdtidt.load_idt();

    CPIC pic;
    pic.init_pic();
    pic.set_interrupt();
    

    CInput_Device dev;
    dev.init_keyboard();
    dev.enable_mouse();
    pic.enable_key_mouse();

    handle_message();

}



void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
