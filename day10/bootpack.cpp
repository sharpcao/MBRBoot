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

    CDesktopLayer* p_desktop = (CDesktopLayer*)ly_mgr.add_layer(CDesktopLayer(pbi->scrnx,pbi->scrny));
    p_desktop->load_img();  
    vga.map(ly_mgr.refresh());
    

    uint mem_size = memtest(0x400000,0xbfffffff);
    stringbuf<50> mem_max_str;
    mem_max_str <<"memory:" << mem_size/(1024*1024) <<" MB";
    //vga.xyprint(5,5,s.c_str());

    auto p_text = ly_mgr.add_layer(CLayer(150,30,20,20));
    p_text->load_img();
    p_text->xyprint(5,5,mem_max_str.c_str());
    vga.map(ly_mgr.update(p_text->get_area()),p_text->get_area());

    auto p_rect = ly_mgr.add_layer(CLayer(150,30,20,20));
    p_rect->load_img(Color8::COL8_000084);

    //p_rect->hide();
    //ly_mgr.set_to_top(*p_text);
    ly_mgr.set_bottom_idx(2);
    ly_mgr.set_to_bottom(*p_rect);
    vga.map(ly_mgr.update(p_rect->get_area()),p_text->get_area());
    



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
