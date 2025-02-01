#include "vga.h"

void CPalette::init_cursor()
{
    for(uint i = 0; i < 16; ++i)
    {
        for(uint j=0; j<16; ++j)
        {
            _cursor_image[i*16+j] = (_cursor_raw[i][j]=='1')? (Color8::COL8_FFFFFF):(Color8::COL8_008484);
        }
    }

}

void CPalette::draw_rect(uint x, uint y,uint width, uint height, char* buf)
{
    for (uint i = 0; i<height; ++i)
    {
        for(uint j = 0; j < width; ++j)
        {
            _vram[(y+i)*_xsize + x + j] = buf[i*width + j];
        }
    }
}

void CPalette::draw_cursor(uint x, uint y)
{
	draw_rect(x,y,16,16,_cursor_image);
}

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
        io_out8(0x03c9,_table_rgb[i*3]);
        io_out8(0x03c9,_table_rgb[i*3+1]);
        io_out8(0x03c9,_table_rgb[i*3+2]);
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

void CPalette::putfont8(uint x, uint y, uchar asc, Color8 col)
{
    uchar* asc_data = _pfont+ asc * 16;
    for(uint i =0; i < 16; ++i)
    {

        char* pos = _vram + (y + i) * get_xsize() + x;
        for(uint j=0; j < 8 ; ++j)
        {
            char b = (0b10000000 >> j);
            if(asc_data[i] & b){
                pos[j] =col;

            }
        }

    }
}
void CPalette::xyprint(uint x, uint y, const char* str, Color8 col)
{
    for(;*str != 0; ++str, x+=8)
    {
        putfont8(x,y,*str,col);
    }
}

void CPalette::draw_desktop()
{
	boxfill(Color8::COL8_008484, 0, 0, _xsize - 1, _ysize - 29);
    boxfill(Color8::COL8_E6E6E6, 0, _ysize - 28, _xsize - 1, _ysize - 28);
    boxfill(Color8::COL8_FFFFFF, 0, _ysize - 27, _xsize - 1, _ysize - 27);
    boxfill(Color8::COL8_E6E6E6, 0, _ysize - 26, _xsize - 1, _ysize - 1);

    boxfill(Color8::COL8_FFFFFF, 3, _ysize - 24, 59, _ysize - 24);
    boxfill(Color8::COL8_FFFFFF, 2, _ysize - 24, 2, _ysize - 4);
    boxfill(Color8::COL8_848484, 3, _ysize - 4, 59, _ysize - 4);
    boxfill(Color8::COL8_848484, 59, _ysize - 23, 59, _ysize - 5);
    boxfill(Color8::COL8_000000, 2, _ysize - 3, 59, _ysize - 3);
    boxfill(Color8::COL8_000000, 60, _ysize - 24, 60, _ysize - 3);
    
    boxfill(Color8::COL8_848484, _xsize - 47, _ysize - 24, _xsize - 4, _ysize - 24);
    boxfill(Color8::COL8_848484, _xsize - 47, _ysize - 23, _xsize - 47, _ysize - 4);
    boxfill(Color8::COL8_FFFFFF, _xsize - 47, _ysize - 3, _xsize - 4, _ysize - 3);
    boxfill(Color8::COL8_FFFFFF, _xsize - 3, _ysize - 24, _xsize - 3, _ysize - 3);


}