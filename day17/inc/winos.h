#ifndef GLOBAL_H
#define GLOBAL_H
#include "common.h"
#include "layer.h"
#include "vga.h"
#include "timer_kit.h"
#include "task_kit.h"
#include "device.h"
#include "handler.h"
#include "functions.h"
#include "winform.h"


class  CWinLayers{
public:
    CWinLayers() = default;
    CDesktopLayer* p_desktop =0;
    //CWindowLayer*  p_window1 =0;
    CTextLayer* p_txt_hello =0;
    CTextLayer* p_txt_key = 0;
    //CTextLayer* p_txt_speedcnt=0;
    CTextLayer* p_txt_timer=0;
    CTextLayer* p_txt_tick=0;
    CInputLayer* p_input = 0;
    CTextLayer* p_debug =0;
    CTextLayer* p_txt_task2 = 0;


};


extern Task_Message_mgr EventList;

class CWinOS{
public:
    CCursor* p_Cursor = 0;
    CMEM_MGR* p_mem_mgr = 0;    
    Layer_mgr* p_layerMgr = 0;
    CWinLayers layers;
    Task_mgr* p_task_mgr = 0;
    CMTTimerCtrl timer_ctrl;
    uint screen_width=0, screen_height=0;
    

    uint _speedcnt = 0;
    uint _speedcnt_task2  = 0;

    static CVGA vga;

    Array<Window*> _windows;
    Window* _active_window = 0;

    template<typename T> 
    T* CreateWindow(uint offset_x, uint offset_y, uint width, uint height);

    bool add_window(Window* wd) { return _windows.push_back(wd);}
    void set_active_window(Window* wd);


    bool post_message(Window* wd, uint msg, uint param);

    void init(const BOOTINFO *pbi, CMEM_MGR& mem_mgr);
    void debug_print(const char* s);
    static char translate_keycode(uint keycode)
    {
        extern char keytable[];
        return (keycode < 0x54)? keytable[keycode] : 0;
    }

private:
    void init_task_mgr(CMEM_MGR& mem_mgr);
    void init_timers(){
        timer_ctrl.init(&EventList);
        timer_ctrl.set_task_mgr(p_task_mgr);
    }
    void init_layers(CMEM_MGR& mem_mgr, uint width, uint height);
    static void _idle_task(uint param);
    
    

};

CVGA CWinOS::vga;
extern CWinOS OS;

template<typename TWin> 
TWin* CWinOS::CreateWindow(uint offset_x, uint offset_y, uint width, uint height)
{
    TWin* p_win = (TWin*)OS.p_mem_mgr->malloc(sizeof(TWin));
    if(!p_win) return 0;

    new(p_win) TWin();

    // p_win->_create_layer(offset_x,  offset_y,  width,  height);
    // p_win->_set_task(Task( OS.p_task_mgr, 
    //                     OS.p_task_mgr->add_task(TWin::task_entry, OS.p_mem_mgr->malloc(8*1024) + 8*1024, (uint)p_win)
    //                 ));
    p_win->InitWindow(offset_x,  offset_y,  width,  height);

    add_window(p_win);
    return p_win;
}

#endif