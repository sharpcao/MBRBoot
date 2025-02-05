#include "vga.h"

void os_main(BOOTINFO *pbi)
{


    CPalette pal(pbi);
    pal.init_palette();
    pal.init_cursor();

    pal.draw_desktop();
    pal.draw_cursor(160,50);

    pal.xyprint(8,8,"ABC 123");
    pal.xyprint(31,31,"Hello World OS!", Color8::COL8_FFFF00);
    //pal.xyprint(30,30,"Haribote OS.", Color8::COL8_FFFFFF);

    for(;;)
    {
        __asm{
            hlt;
        }
    }

}

void (*jump)(BOOTINFO *pbi)  = os_main;
