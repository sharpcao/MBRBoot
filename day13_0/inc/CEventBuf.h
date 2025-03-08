#ifndef CEVENTBUF_H
#define CEVENTBUF_H

#include "common.h"
template<uint N>
class CEventBuf
{

public:
	
	bool set_char(char c);
	bool get_char(char* c);
	bool is_empty() { return (_i1 == _i2);}

private:
	uint _i1=0, _i2 = 0;
	char _data[N] ={0};
	
};

template<uint N>
bool CEventBuf<N>::set_char(char c)
{
	if ( (_i2 + 1 ) % N == _i1 ){
		return false;
	}else{
		_data[_i2++] = c;
		if (_i2 == N) {_i2 = 0;}
		return true;
	}
}

template<uint N>
bool CEventBuf<N>::get_char(char* p_char)
{
	*p_char = 0;
	if (_i1 != _i2){
		*p_char = _data[_i1++];
		if(_i1 == N) {_i1 = 0;}
		if(_i1 == _i2) {_i1 = _i2 = 0;}
		return true;
	}
	return false;
}


class CMouseDecode{

public:
    int dx=0,dy=0,btn=0;
    int scrn_x=0,scrn_y=0;
    bool push_char(uchar c);  // return ture if decode complete

};

bool CMouseDecode::push_char(uchar c)
{
    
    static uchar data[4] = {0};
    static int phase = -1;
    if (phase <0) {
        if(c == 0xfa) phase = 0;
        return false;
    }

    if(++phase > 3) phase = 1;

    if((phase == 1) && ((c & 0xc8) != 0x08)){
        phase = 0;
        return false;
    }

    data[phase] = c;

    if(phase == 3){
        btn = data[1];
        dx = data[2];
        dy = data[3];
        if ((btn & 0x10) != 0) dx|= 0xffffff00;
        if ((btn & 0x20) != 0) dy|= 0xffffff00;
        dy = -dy;
        return true;
    }
   
    return false;
}

#endif