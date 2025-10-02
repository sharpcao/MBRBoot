#include "inc\winos.h"

/*
char keytable[0x54] = {
0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0, 0,
'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0, 0, 'A', 'S',
'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0, 0, ']', 'Z', 'X', 'C', 'V',
'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
'2', '3', '0', '.'
};
*/

char keytable[0x80] = {
0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',
'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
'2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0x5c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0
};

char keytable_UP[0x80] = {
0, 0, '!', '@', '#', '$', '%', '^', '&', '*','(', ')', '_', '+', 0, 0, 
'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',
'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V',
'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
'2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, '_', 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0,0
};


char TransKey::translate_keycode(uint keycode)
{
    if(keycode < 0x80){
       return (TransKey::is_shift())? keytable_UP[keycode] : keytable[keycode]; 
    }
    return 0;   
}



void lymgr_update(const Layer_mgr& lymgr, const CRect& rect);


void CWinOS::debug_print(const char* s)
{
  
    auto pd = layers.p_debug;
    if(pd){
        pd->set_text(s);
        CWinOS::vga.map((*p_layerMgr)[0]->get_mem(),pd->get_mem(),pd->get_area());
    }
}

void CWinOS::init_task_mgr(CMEM_MGR& mem_mgr) 
{
    p_task_mgr = (Task_mgr*) mem_mgr.malloc(sizeof(Task_mgr));
    new(p_task_mgr) Task_mgr;


    p_task_mgr->set_active( p_task_mgr->add_task(_idle_task, mem_mgr.malloc(8*1024) + 8*1024), 
                            Task_mgr::PT::low, Task_mgr::LV::level_1
                           );
    
}

void CWinOS::init(const BOOTINFO *pbi, CMEM_MGR& mem_mgr)
{
    p_mem_mgr = &mem_mgr;
    vga.init(pbi);
    init_layers(mem_mgr, pbi->scrnx,pbi->scrny);
    init_task_mgr(mem_mgr);

    EventList.set_msg_task(p_task_mgr->get_task(0),p_task_mgr);
    init_timers();

    GDTIDT gdtidt;

    gdtidt.add_idt_handler(0x20, handler_wrap<int20_handler>);
    gdtidt.add_idt_handler(0x21, handler_wrap<int21_handler>);
    gdtidt.add_idt_handler(0x2c, handler_wrap<int2c_handler>);
    gdtidt.add_idt_handler(0x07, handler_wrap<int7_handler>);

    
    CPIC pic;
    pic.init_pic();
    pic.set_interrupt();

  
    CPIT pit;
    pit.init_pit();

    CInput_Device dev;
    dev.init_keyboard();
    dev.enable_mouse();
    pic.enable_key_mouse();



}

void CWinOS::_idle_task(uint param)
{
    for(;;)
    {
        io_hlt();
    }
}

void CWinOS::init_layers(CMEM_MGR& mem_mgr, uint width, uint height)
{
    screen_width = width;
    screen_height = height;
    
    p_Cursor = (CCursor*) mem_mgr.malloc(sizeof(CCursor));
    new(p_Cursor) CCursor(width / 2, height/2,width,height);

    p_layerMgr = (Layer_mgr*)mem_mgr.malloc(sizeof(Layer_mgr));
    new(p_layerMgr) Layer_mgr(mem_mgr, width, height);

    Layer_mgr& ly_mgr = *p_layerMgr;

    ly_mgr._updateEvent = lymgr_update;

    layers.p_desktop =  (CDesktopLayer*)ly_mgr.create_layer(CDesktopLayer(width,height));
    layers.p_desktop->load_img();
 

    p_layerMgr->set_bottom_idx(2);

    layers.p_txt_hello = (CTextLayer* )ly_mgr.create_layer(CTextLayer(0,0,150,30));
    layers.p_txt_hello->set_text("Hello OS!");

    layers.p_txt_key = (CTextLayer* )ly_mgr.create_layer(CTextLayer(0,20,150,30));
    layers.p_txt_key->set_text("Key");
  
    
    layers.p_txt_timer = (CTextLayer* )ly_mgr.create_layer(CTextLayer(0,40,120,20));
    layers.p_txt_timer->set_text("wait 5 s");
 

    layers.p_txt_tick = (CTextLayer* )ly_mgr.create_layer(CTextLayer(width - 40,height-20,120,20));
    layers.p_txt_tick->set_text("tick",Color8::COL8_000000);

    layers.p_input = (CInputLayer*)ly_mgr.create_layer(CInputLayer(70,20,100,20));
    layers.p_input->load_img(Color8::COL8_FFFFFF);
    

    layers.p_debug = (CTextLayer* )ly_mgr.create_layer(CTextLayer(100,0,120,20));
    layers.p_debug->set_text("DEBUG",Color8::COL8_FF0000);
    //layers.p_debug->hide();

    layers.p_txt_task2 = (CTextLayer* )ly_mgr.create_layer(CTextLayer(0,150,120,20));
    layers.p_txt_task2->set_text("task2",Color8::COL8_FF0000);


    // layers.p_window1 = (CWindowLayer*)ly_mgr.create_layer(CWindowLayer(width * 0.65, 2,width * 0.35,height *0.45,0));
    // layers.p_window1->load_img("Window");
    
}


bool CWinOS::post_message(Window* wd, uint msg, uint param)
{
    bool result = false;
    if(_windows.has(wd)) {
        result = true;
        if (msg == EVENT::Actived){
            wd->set_active();
        }


        wd->push_message(msg, param);
    }
    return result;
}

void CWinOS::set_active_window(Window* wd)
{
    
}



void lymgr_update(const Layer_mgr& lymgr, const CRect& rect)
{
    CWinOS::vga.map(lymgr[0]->get_mem(), rect);
}