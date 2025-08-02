#ifndef COMMOM_H
#define COMMOM_H

#ifdef WIN32D
#include <iostream>
#include <string>
#define DEBUG_BUILD(TXT) {TXT}
#define DEBUG_PRINT(TXT) {std::cout << (TXT) << std::endl;}
#else
#define DEBUG_BUILD(TXT) 
#define DEBUG_PRINT(TXT) 
#endif

using uint = unsigned int;
using uchar = unsigned char;

constexpr uint CURSOR_WIDTH = 16;
constexpr uint CURSOR_HEIGHT = 16;

#ifndef WIN32D
inline void* operator new(uint, void* ptr) noexcept { return ptr; }
#endif

template<typename T>
inline T min(T a, T b)
{
	return (a<b)? a:b;
}

template<typename T>
inline T max(T a, T b)
{
	return (a>b)? a:b;
}

template<typename T>
T set_inrange(T v, T low, T high)
{
    return (v < low)? low :((v>high)? high :v);
}


class CRect{
public:
	uint _x,_y,_w, _h;
public:
	CRect(uint x=0, uint y=0, uint w=0, uint h=0) : _x(x),_y(y),_w(w),_h(h) {}
	void set(uint x, uint y, uint w, uint h){
		_x = x;
		_y =y;
		_w = w;
		_h = h;
	}
	CRect to_vga_pos(uint offset_x, uint offset_y){
		return CRect(offset_x + _x, offset_y + _y, _w, _h);
	}
	uint x1() const {return _x + _w;}
	uint y1() const{return _y + _h;}
 	uint size() const{ return _w*_h;}
	void reset() {_x=_y=_w=_h = 0;}
	CRect& operator=(const CRect&)=default;
};


enum EVENT{
    Key = 1,
    Mouse = 2,
    Timer = 3,
    Actived,
    Deactived
};


#endif