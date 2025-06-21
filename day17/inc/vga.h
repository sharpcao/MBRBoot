#ifndef VGA_H
#define VGA_H
#include "common.h"
#include "os_io.h"

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
    COL8_848484=15,
    COL8_TP=99

};

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx, scrny;
    char *vram;
};

class CCursor{
public:
    uint _x, _y;
    uint _xlim, _ylim;
    char _cursor_image[CURSOR_WIDTH * CURSOR_HEIGHT];
    
public:
    CCursor(uint x, uint y, uint xlim, uint ylim) :_x(x),_y(y),_xlim(xlim),_ylim(ylim) {
        load_image();
    }
    void move_cursor_by(int dx, int dy);
    char* get_image() {return _cursor_image;}
    char get_abs_color(uint ix, uint iy){
        int x = ix - _x;
        int y = iy - _y;
        if ( (x >= 0) && (x < CURSOR_WIDTH) && (y >= 0) && (y < CURSOR_HEIGHT))
        {
            return _cursor_image[y*CURSOR_WIDTH + x];
        }else{
            return Color8::COL8_TP;
        }
    }

private:
    void load_image();
};

class CVGA
{
public:
    void init(const BOOTINFO *pbi) 
    {
        _pboot_info = pbi;
        _vram = _pboot_info->vram;
        _xsize = _pboot_info->scrnx;
        _ysize = _pboot_info->scrny;
        _init_palette();

    }
                
    CVGA() {};
    
    void fill(uint x, uint y,uint width, uint height, 
                                Color8 color=Color8::COL8_008484, char* buf=0);

    
    void map(const char* pic, const CRect& rt) {
        _map(rt._x,rt._y,rt._w,rt._h,pic);
    }
    void map(const char* back, const char* front, const CRect& rt) {
        _map(rt._x,rt._y,rt._w,rt._h,back,front);
    }
    void map(const char* pic){
        _map(0,0,_xsize,_ysize,pic);
    }


    void putfont8(uint x, uint y, uchar asc, Color8 col = Color8::COL8_FFFFFF);
    void xyprint(uint x, uint y, const char* str, Color8 col = Color8::COL8_FFFFFF);
    
    uint get_xsize() {return _xsize;}
    uint get_ysize() {return _ysize;} 
    char* get_vram() {return _vram;}

    static uchar* get_font()  { return _pfont;}
    
private:
    void _init_palette();
    void _map(uint x, uint y, uint width, uint height, const char* back, const char* buf=0);
    static const BOOTINFO* _pboot_info;
    static char* _vram;
    static uint _xsize, _ysize;  
    static uchar* _pfont;   
    static uchar _table_rgb[];

};

const BOOTINFO* CVGA::_pboot_info = 0;
char* CVGA::_vram =0;
uint CVGA::_xsize =0, CVGA::_ysize=0;
extern uchar hankaku_font[];
uchar* CVGA::_pfont = hankaku_font;

uchar CVGA::_table_rgb[] = {
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