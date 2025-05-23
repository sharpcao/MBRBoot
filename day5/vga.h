#ifndef VGA_H
#define VGA_H

#include "os_io.h"

using uint = unsigned int;
using uchar = unsigned char;

enum Color8 {
    COL8_000000=0,
    COL8_FF0000=1,
    COL8_00FF00=2,
    COL8_FFFF00=3,
    COL8_0000FF=4,
    COL8_FF00FF=5,
    COL8_00FFFF=6,
    COL8_FFFFFF=7,
    COL8_E6E6E6=8,
    COL8_840000=9,
    COL8_008400=10,
    COL8_848400=11,
    COL8_000084=12,
    COL8_840084=13,
    COL8_008484=14,
    COL8_848484=15

};

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx, scrny;
    char *vram;
};


class CPalette
{
public:
    CPalette(BOOTINFO *pbi) : _pboot_info(pbi),
                              _vram(_pboot_info->vram) ,
                              _xsize(_pboot_info->scrnx),
                              _ysize(_pboot_info->scrny) {}
                
    void init_palette();
    void init_cursor();

    void boxfill(Color8 color, uint x0, uint y0, uint x1, uint y1);
    void draw_desktop();

    void putfont8(uint x, uint y, uchar asc, Color8 col = Color8::COL8_FFFFFF);
    void xyprint(uint x, uint y, const char* str, Color8 col = Color8::COL8_FFFFFF);

    void draw_cursor(uint x, uint y);
    void draw_rect(uint x, uint y,uint width, uint height, char* buf);
    uint get_xsize() {return _xsize;}
    uint get_ysize() {return _ysize;}


private:
    BOOTINFO* _pboot_info;
    char* _vram;
    uint _xsize, _ysize;

    static uchar _table_rgb[] ;
    static uchar* _pfont;
    static char* _cursor_raw[16];
    static char _cursor_image[16*16];

};

extern uchar hankaku_font[];
uchar* CPalette::_pfont = hankaku_font;
char* CPalette::_cursor_raw[16] = {
        "*11111111111*...",
        "*1111111111*....",
        "*111111111*.....",
        "*11111111*......",
        "*1111111*.......",
        "*1111111*.......",
        "*11111111*......",
        "*1111**111*.....",
        "*111*..*111*....",
        "*11*....*111*...",
        "*1*......*111*..",
        "**........*111*.",
        "*..........*111*",
        "............*11*",
        ".............***"
};
char CPalette::_cursor_image[16*16] = {0};

uchar CPalette::_table_rgb[] = {
        0x00,0x00,0x00,   //0:黑 
        0xff, 0x00, 0x00, //1:亮红 
        0x00, 0xff, 0x00, //2:亮绿 
        0xff, 0xff, 0x00, //3:亮黄 
        0x00, 0x00, 0xff, //4:亮蓝 
        0xff, 0x00, 0xff, //5:亮紫 
        0x00, 0xff, 0xff, //6:浅亮蓝 
        0xff, 0xff, 0xff, //7:白 
        0xe6, 0xe6, 0xe6, //8:亮灰 
        0x84, 0x00, 0x00, //9:暗红 
        0x00, 0x84, 0x00, //10:暗绿 
        0x84, 0x84, 0x00, //11:暗黄 
        0x00, 0x00, 0x84, //12:暗青 
        0x84, 0x00, 0x84, //13:暗紫 
        0x00, 0x84, 0x84, //14:浅暗蓝 
        0x84, 0x84, 0x84  //15:暗灰 
};

#endif