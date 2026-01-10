#ifndef DECODE_KIT_H
#define DECODE_KIT_H

class Mouse_decode{

public:
    int dx=0,dy=0,btn=0;
    int scrn_x=0,scrn_y=0;
    bool push_char(uchar c);  // return ture if decode complete

};

bool Mouse_decode::push_char(uchar c)
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