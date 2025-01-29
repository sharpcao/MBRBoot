#include "bootpack.h"

void os_main()
{

    CPalette pal;
    pal.init_palette();
    // pal.boxfill(Color8::COL8_FF0000,20, 20, 120, 120);
    // pal.boxfill(Color8::COL8_00FF00,70, 50, 170, 150);
    // pal.boxfill(Color8::COL8_0000FF,120, 80, 220, 180);
    uint xsize = pal.get_xsize();
    uint ysize = pal.get_ysize();

    pal.boxfill(Color8::COL8_008484, 0, 0, xsize - 1, ysize - 29);
    pal.boxfill(Color8::COL8_E6E6E6, 0, ysize - 28, xsize - 1, ysize - 28);
    pal.boxfill(Color8::COL8_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27);
    pal.boxfill(Color8::COL8_E6E6E6, 0, ysize - 26, xsize - 1, ysize - 1);

    pal.boxfill(Color8::COL8_FFFFFF, 3, ysize - 24, 59, ysize - 24);
    pal.boxfill(Color8::COL8_FFFFFF, 2, ysize - 24, 2, ysize - 4);
    pal.boxfill(Color8::COL8_848484, 3, ysize - 4, 59, ysize - 4);
    pal.boxfill(Color8::COL8_848484, 59, ysize - 23, 59, ysize - 5);
    pal.boxfill(Color8::COL8_000000, 2, ysize - 3, 59, ysize - 3);
    pal.boxfill(Color8::COL8_000000, 60, ysize - 24, 60, ysize - 3);
    
    pal.boxfill(Color8::COL8_848484, xsize - 47, ysize - 24, xsize - 4, ysize - 24);
    pal.boxfill(Color8::COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize - 4);
    pal.boxfill(Color8::COL8_FFFFFF, xsize - 47, ysize - 3, xsize - 4, ysize - 3);
    pal.boxfill(Color8::COL8_FFFFFF, xsize - 3, ysize - 24, xsize - 3, ysize - 3);


    for(;;)
    {
        __asm{
            hlt;
        }
    }

}

void (*jump)()  = os_main;