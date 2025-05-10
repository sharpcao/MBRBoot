#ifndef GLOBAL_H
#define GLOBAL_H
#include "common.h"
#include "layer.h"
#include "vga.h"
#include "timer.h"

//extern CEventBuf<64> TimerBuf; 

class  CWinLayers{
public:
    CWinLayers() = default;
    CDesktopLayer* p_desktop =0;
    CWindowLayer*  p_window1 =0;
    CTextLayer* p_txt_hello =0;
    CTextLayer* p_txt_key = 0;
    //CTextLayer* p_txt_speedcnt=0;
    CTextLayer* p_txt_timer=0;
    CTextLayer* p_txt_tick=0;
    CInputLayer* p_input = 0;
    CTextLayer* p_debug =0;
    CTextLayer* p_txt_task2 = 0;

};


extern CEventBuf<> EventList;

class CWinOS{
public:
    CCursor* p_Cursor = 0;    
    CLayer_Mgr* p_layerMgr = 0;
    CWinLayers layers;
    CTimerCtrl timer_ctrl;
    uint _speedcnt = 0;
    uint _speedcnt_task2  = 0;

    static CVGA vga;

    void init_timers(){
        timer_ctrl.init(&EventList);
    }
    void init_layers(CMEM_MGR& mem_mgr, uint width, uint height);

    void debug_print(const char* s);

};

CVGA CWinOS::vga;








#endif