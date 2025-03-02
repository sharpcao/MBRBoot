#ifndef GLOBAL_H
#define GLOBAL_H
#include "common.h"
#include "layer.h"
#include "vga.h"
#include "timer.h"

extern CEventBuf<64> TimerBuf; 

class  CWinLayers{
public:
    CWinLayers() = default;
    CDesktopLayer* p_desktop =0;
    CWindowLayer*  p_window1 =0;
    CTextLayer* p_txt_hello =0;
    CTextLayer* p_txt_counter=0;
    CTextLayer* p_txt_timer=0;
    CTextLayer* p_debug =0;

};

class C_WOS {
public:
    CCursor* p_Cursor = 0;    
    CLayer_Mgr* p_layerMgr = 0;
    CWinLayers layers;
   // CTimer	timer;
    CTimerCtrl timer_ctrl;
    void init(){
        timer_ctrl.init(&TimerBuf);
    }
    void debug_print(const char* s){
        static CVGA vga;
        auto pd = layers.p_debug;
        if(pd){
            pd->set_text(s);
            vga.map((*p_layerMgr)[0]->get_mem(),pd->get_mem(),pd->get_area());
        }
    }
};




#endif