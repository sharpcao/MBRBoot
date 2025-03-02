#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "..\inc\vga.h"
#include "..\inc\layer.h"
#include "..\inc\mem_mgr.h"
using namespace std;


C_WOS OS;
int main()
{
	BOOTINFO bi, *pbi;
	bi.scrnx = 320;
	bi.scrny = 200;
	bi.vram = (char*)malloc(320*200);


	pbi = &bi;


    CVGA vga(pbi);
    //vga.init_palette();

    //CMEM_MGR mem_mgr(0x200000,0x100000);
    CMEM_MGR mem_mgr((uint)malloc(0x100000),0x100000);

    OS.p_layerMgr = (CLayer_Mgr*)mem_mgr.malloc(sizeof(CLayer_Mgr));
    new(OS.p_layerMgr) CLayer_Mgr(mem_mgr,pbi->scrnx,pbi->scrny);
  	CLayer_Mgr& ly_mgr = *OS.p_layerMgr;

    OS.p_Cursor = (CCursor*) mem_mgr.malloc(sizeof(CCursor));
    new(OS.p_Cursor) CCursor(vga.get_xsize()/2 ,vga.get_ysize()/2, vga.get_xsize(),vga.get_ysize());


    CDesktopLayer* p_desktop = (CDesktopLayer*)ly_mgr.add_layer(CDesktopLayer(pbi->scrnx,pbi->scrny));
    p_desktop->load_img();  
    vga.map(ly_mgr.refresh());
    ly_mgr.set_bottom_idx(2);
    

    CWindowLayer* p_window = (CWindowLayer*)ly_mgr.add_layer(CWindowLayer(150,100,100,40));
    p_window->load_img("Window");
    p_window->xyprint(5,30,"mem_max_str.c_str()",Color8::COL8_000000);
    vga.map(ly_mgr.update(p_window->get_area()),p_window->get_area());
   


    CTextLayer* p_count = (CTextLayer* )ly_mgr.add_layer(CTextLayer(80,16,0,20));
    p_count->set_text("Count");
    p_count->hide();
    ly_mgr.update(p_count->get_area());
    
    char s[40];
    uint count;
   	clock_t start, end; 
   	CRect rt= p_count->get_area();
   	printf("(%d,%d,%d,%d)\n",rt._x,rt._y,rt._w,rt._h);
    start = clock();
    for(count = 1; count <=1000*1000; ++count){
        uint2str(s,count);
        p_count->set_text(s,Color8::COL8_FFFF00);
        vga.map(ly_mgr[0]->get_mem(),p_count->get_mem(),p_count->get_area());

    }
	end = clock();
	printf("time=%f\n", (double)(end - start) / CLOCKS_PER_SEC);
}


