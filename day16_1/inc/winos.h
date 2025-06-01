#ifndef GLOBAL_H
#define GLOBAL_H
#include "common.h"
#include "layer.h"
#include "vga.h"
#include "timer_kit.h"
#include "task_kit.h"
#include "device.h"
#include "handler.h"



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


extern Message_mgr<> EventList;

class CWinOS{
public:
    CCursor* p_Cursor = 0;    
    CLayer_Mgr* p_layerMgr = 0;
    CWinLayers layers;
    Task_mgr* p_task_mgr = 0;
    CMTTimerCtrl timer_ctrl;
    uint _speedcnt = 0;
    uint _speedcnt_task2  = 0;
    static CVGA vga;

    void init(const BOOTINFO *pbi, CMEM_MGR& mem_mgr);
    void debug_print(const char* s);

private:
    void init_task_mgr(CMEM_MGR& mem_mgr) {
        p_task_mgr = (Task_mgr*) mem_mgr.malloc(sizeof(Task_mgr));
        new(p_task_mgr) Task_mgr;
    }
    void init_timers(){
        timer_ctrl.init(&EventList);
        timer_ctrl.set_task_mgr(p_task_mgr);
    }
    void init_layers(CMEM_MGR& mem_mgr, uint width, uint height);

    

};

CVGA CWinOS::vga;



#endif