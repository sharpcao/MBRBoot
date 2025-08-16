#include "inc\console.h"
#include "inc\os_io.h"
#include "inc\functions.h"

extern CWinOS OS;


void ConsoleLayer::twinkle()
{
	static bool bshow = false;
	Color8 col = bshow ? Color8::COL8_000000 : Color8::COL8_FFFFFF;

	CRect _twinkle_box = cursor_client_box().add_offset(_client_offset_x, _client_offset_y);
	fill_box(col, _twinkle_box);
	OS.p_layerMgr->update(_twinkle_box.to_vga_pos(_offset_x, _offset_y));
	bshow = !bshow;
}


void ConsoleLayer::add_char(uchar asc)
{
	if(asc){
		CRect cursor_box = cursor_client_box().add_offset(_client_offset_x, _client_offset_y);
		fill_box(Color8::COL8_000000, cursor_box);
		putfont8(cursor_box._x, cursor_box._y,asc, COL8_FFFFFF);
		++_cursor_col;
	}
}

void ConsoleWindow::_create_layer(uint offset_x, uint offset_y, uint width, uint height)
{
	win_layer = (ConsoleLayer*)OS.p_layerMgr->create_layer(
                                         ConsoleLayer(offset_x,offset_y,width,height,this));
	stringbuf<> s("console1");

	static_cast<ConsoleLayer*>(win_layer)->load_img(s.c_str());
	
}

ConsoleWindow::~ConsoleWindow()
{
	if(win_layer){
		OS.p_layerMgr->remove_layer(win_layer);
	}
}



void console_timeout(uint tmr)
{

	OS.timer_ctrl.set_timer(tmr,80,console_timeout);

}

void ConsoleWindow::Run()
{
	//active();
	ConsoleLayer* consoleLayer =  reinterpret_cast<ConsoleLayer*>(win_layer);
	
	consoleLayer->set_title(0, *OS.p_layerMgr);
	
	OS.timer_ctrl.add_timer(80,console_timeout, &message_mgr);
	uint p1,p2;
	for(;;)
	{
		io_cli();
		if(!message_mgr.get_message(p1,p2))
		{
			message_mgr.deact_msg_task();
			io_sti();
		}else{
			io_sti();
			if( p1 == EVENT::Timer){
				consoleLayer->twinkle();
                OS.timer_ctrl.call_hander((uint)p2);
            }else if(p1 == EVENT::Actived){
            	//active();	
            }else if(p1 == EVENT::Key){
           		uchar c = p2;
           		if(c) {
           			consoleLayer->add_char(c);
	            	OS.p_layerMgr->update(consoleLayer->get_area());
	            }

            }
		}

	}

}


void ConsoleWindow::redraw()
{
	reinterpret_cast<ConsoleLayer*>(win_layer)->load_img(0);
}