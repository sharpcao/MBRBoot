#ifndef BOOTPACK_H
#define BOOTPACK_H

#include "os_io.h"

using uint = unsigned int;


class CPalette
{
public:
    void init_palette();
    CPalette() = default;
private:
    static unsigned char table_rgb[] ;

};

void CPalette::init_palette()
{
    constexpr uint start = 0;
    constexpr uint size = 16; 
    int i, eflags;
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


unsigned char CPalette::table_rgb[] = {
        0x00,0x00,0x00,   //0:黑 
        0xff, 0x00, 0x00, //1:亮红 
        0x00, 0xff, 0x00, //2:亮绿 
        0xff, 0xff, 0x00, //3:亮黄 
        0x00, 0x00, 0xff, //4:亮蓝 
        0xff, 0x00, 0xff, //5:亮紫 
        0x00, 0xff, 0xff, //6:浅亮蓝 
        0xff, 0xff, 0xff, //7:白 
        0xc6, 0xc6, 0xc6, //8:亮灰 
        0x84, 0x00, 0x00, //9:暗红 
        0x00, 0x84, 0x00, //10:暗绿 
        0x84, 0x84, 0x00, //11:暗黄 
        0x00, 0x00, 0x84, //12:暗青 
        0x84, 0x00, 0x84, //13:暗紫 
        0x00, 0x84, 0x84, //14:浅暗蓝 
        0x84, 0x84, 0x84  //15:暗灰 
};




#endif