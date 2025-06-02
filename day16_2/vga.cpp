#include "inc\vga.h"
#include "inc\layer.h"
#include "inc\functions.h"
#include "inc\winos.h"

const char* RES_CUROSR[16] = {
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
        "............**1*",
        ".............***"
};

const char* RES_BTN_CLOSE[14] ={
        "OOOOOOOOOOOOOOO@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQ@@QQQQ@@QQ$@",
        "OQQQQ@@QQ@@QQQ$@",
        "OQQQQQ@@@@QQQQ$@",
        "OQQQQQQ@@QQQQQ$@",
        "OQQQQQ@@@@QQQQ$@",
        "OQQQQ@@QQ@@QQQ$@",
        "OQQQ@@QQQQ@@QQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@",
        "@@@@@@@@@@@@@@@@"
};

extern CWinOS OS;

void CVGA::fill(uint x, uint y,uint width, uint height, Color8 color, char* buf)
{
    for (uint i = 0; i<height; ++i)
    {
        if (y + i >= _ysize) break;
        for(uint j = 0; j < width; ++j)
        {
            if(x + j >= _xsize) break;
            _vram[(y+i)*_xsize + x + j] = (buf)?buf[i*width + j]:color;
        }
    }
}


void CVGA::_map(uint x, uint y, uint width, uint height, const char* back, const char* buf)
{
    Color8 color;
    CCursor& cursor = *OS.p_Cursor;
    uint xlim = min(_xsize,x+width);
    uint ylim = min(_ysize,y+height);
    bool bcursor_in_area = true;
    if((x < cursor._x && x+width < cursor._x) ||(x>cursor._x && x>cursor._x+16))
    {
        bcursor_in_area = false;
    }
    else if((y < cursor._y && y +height < cursor._y) ||(y>cursor._y && y> cursor._y + 16) )
    {
        bcursor_in_area = false;
    }

    for(uint i=y; i < ylim; ++i){
        for(uint j=x; j < xlim; ++j){
           if(bcursor_in_area &&(color = (Color8)cursor.get_abs_color(j,i))!=Color8::COL8_TP) goto set_color;
           if(buf && ((color = (Color8)buf[(i-y)*width+j-x])!=Color8::COL8_TP) ) goto set_color;
           if( (color = (Color8)back[i*_xsize+j])!=Color8::COL8_TP) goto set_color;
           continue;
           // color = Color8::COL8_FFFFFF;
set_color:           
            _vram[i*_xsize+j] = color;
        }
    }


}

void CVGA::_init_palette()
{
    constexpr uint start = 0;
    constexpr uint size = 16; 
    uint i, eflags;
    eflags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, start);
    for(uint i = start; i < size; ++i)
    {
        io_out8(0x03c9,_table_rgb[i*3]/4);
        io_out8(0x03c9,_table_rgb[i*3+1]/4);
        io_out8(0x03c9,_table_rgb[i*3+2]/4);
    }
    io_store_eflags(eflags);

}


void CVGA::putfont8(uint x, uint y, uchar asc, Color8 col)
{
    uchar* asc_data = _pfont+ asc * 16;
    for(uint i =0; i < 16; ++i)
    {
        char* pos = _vram + (y + i) * _xsize + x;
        for(uint j=0; j < 8 ; ++j)
        {
            char b = (0b10000000 >> j);
            if(asc_data[i] & b) { pos[j] = col;}
        }

    }
}

void CVGA::xyprint(uint x, uint y, const char* str, Color8 col)
{
    for(;*str != 0; ++str, x+=8)
        putfont8(x,y,*str,col);

}


void CCursor::load_image()
{
    for(uint i = 0; i < 16; ++i)
        for(uint j=0; j<16; ++j)
            CCursor::_cursor_image[i*16+j] = (RES_CUROSR[i][j]=='1')? (Color8::COL8_FFFFFF):(Color8::COL8_TP);

}

void CCursor::move_cursor_by(int dx, int dy)
{
    _x = set_inrange<int>(_x+dx,0,_xlim);
    _y = set_inrange<int>(_y+dy,0,_ylim);
}
