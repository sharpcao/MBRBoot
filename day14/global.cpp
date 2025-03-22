#include "inc\global.h"


char keytable[0x54] = {
0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0, 0,
'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0, 0, 'A', 'S',
'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0, 0, ']', 'Z', 'X', 'C', 'V',
'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
'2', '3', '0', '.'
};


void lymgr_update(const CLayer_Mgr& lymgr, const CRect& rect);

void CWinOS::debug_print(const char* s)
{
  
    auto pd = layers.p_debug;
    if(pd){
        pd->set_text(s);
        CWinOS::vga.map((*p_layerMgr)[0]->get_mem(),pd->get_mem(),pd->get_area());
    }
}


void CWinOS::init_layers(CMEM_MGR& mem_mgr, uint width, uint height)
{
    
    p_Cursor = (CCursor*) mem_mgr.malloc(sizeof(CCursor));
    new(p_Cursor) CCursor(width / 2, height/2,width,height);

    p_layerMgr = (CLayer_Mgr*)mem_mgr.malloc(sizeof(CLayer_Mgr));
    new(p_layerMgr) CLayer_Mgr(mem_mgr, width, height);

    CLayer_Mgr& ly_mgr = *p_layerMgr;

    ly_mgr._updateEvent = lymgr_update;

    layers.p_desktop =  (CDesktopLayer*)ly_mgr.add_layer(CDesktopLayer(width,height));
    layers.p_desktop->load_img();
 

    p_layerMgr->set_bottom_idx(2);

    layers.p_txt_hello = (CTextLayer* )ly_mgr.add_layer(CTextLayer(0,0,150,30));
    layers.p_txt_hello->set_text("Hello OS!");

    layers.p_txt_key = (CTextLayer* )ly_mgr.add_layer(CTextLayer(0,20,150,30));
    layers.p_txt_key->set_text("Key");
  
    
    layers.p_txt_timer = (CTextLayer* )ly_mgr.add_layer(CTextLayer(0,40,120,20));
    layers.p_txt_timer->set_text("wait 5 s");
 

    layers.p_txt_tick = (CTextLayer* )ly_mgr.add_layer(CTextLayer(width - 40,height-20,120,20));
    layers.p_txt_tick->set_text("tick",Color8::COL8_000000);

    layers.p_input = (CInputLayer*)ly_mgr.add_layer(CInputLayer(70,20,100,20));
    layers.p_input->load_img(Color8::COL8_FFFFFF);
    

    layers.p_debug = (CTextLayer* )ly_mgr.add_layer(CTextLayer(100,0,120,20));
    layers.p_debug->set_text("DEBUG",Color8::COL8_FF0000);
    //layers.p_debug->hide();


    layers.p_window1 = (CWindowLayer*)ly_mgr.add_layer(CWindowLayer(width/4,height/4,width/2,height/2));
    layers.p_window1->load_img("Window");
    
}


void lymgr_update(const CLayer_Mgr& lymgr, const CRect& rect)
{
    CWinOS::vga.map(lymgr[0]->get_mem(), rect);
}