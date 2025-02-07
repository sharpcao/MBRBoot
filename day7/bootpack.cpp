#include <cstdio>
#include "vga.h"
#include "int.h"
#include "input_device.h"
#include "handler.h"

void os_main(BOOTINFO *pbi)
{

    CPalette pal(pbi);
    pal.init_palette();
    pal.init_cursor();
    pal.draw_desktop();
    pal.draw_cursor(160,50);

    GDTIDT gdtidt;
    gdtidt.init_gdt_idt();  
    gdtidt.add_idt_handler(0x21, handler_wrap<int21_handler>);
    gdtidt.add_idt_handler(0x2c, handler_wrap<int2c_handler>);
    gdtidt.load_gdt();
    gdtidt.load_idt();

    CPIC pic;
    pic.init_pic();
    pic.set_interrupt();
    

    pal.xyprint(5,5,"ABC 123");
    pal.xyprint(28,28,"Hello World OS!", Color8::COL8_FFFF00);

    CInput_Device dev;
    dev.init_keyboard();
    dev.enable_mouse();
    pic.enable_key_mouse();

    handle_message();

}



void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
