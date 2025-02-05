#ifndef BOOTPACK_H
#define BOOTPACK_H

#include "os_io.h"

using uint = unsigned int;

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
    CPalette(BOOTINFO *pbi) : _pboot_info(pbi), _vram(_pboot_info->vram) {}
    CPalette() {};
    void init_palette();
    void boxfill(Color8 color, uint x0, uint y0, uint x1, uint y1);
    uint get_xsize() {return _pboot_info->scrnx;}
    uint get_ysize() {return _pboot_info->scrny;}

private:
    static BOOTINFO* _pboot_info;
    static char* _vram;

    static unsigned char table_rgb[] ;

};

void CPalette::init_palette()
{
    constexpr uint start = 0;
    constexpr uint size = 16; 
    uint i, eflags;
    eflags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, start);
    for(uint i = start; i < size; ++i)
    {
        io_out8(0x03c9,table_rgb[i*3]);
        io_out8(0x03c9,table_rgb[i*3+1]);
        io_out8(0x03c9,table_rgb[i*3+2]);
    }
    io_store_eflags(eflags);

}

void CPalette::boxfill(Color8 color, uint x0, uint y0, uint x1, uint y1)
{
    uint _xsize = get_xsize();
    for(uint y = y0; y <= y1; ++y)
        for(uint x = x0; x <= x1; ++x)
            _vram[y*_xsize + x] = color;

}

unsigned char CPalette::table_rgb[] = {
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