#include "inc\console.h"
#include "inc\os_io.h"

extern CWinOS OS;


void ConsoleLayer::twinkle()
{
	static bool bshow = false;
	Color8 col = bshow ? Color8::COL8_000000 : Color8::COL8_FFFFFF;
	fill_box(col, _twinkle_box);
	OS.p_layerMgr->update(_twinkle_box.to_vga_pos(_offset_x, _offset_y));
	bshow = !bshow;
}

void ConsoleWindow::_create_layer(uint offset_x, uint offset_y, uint width, uint height)
{
	win_layer = (ConsoleLayer*)OS.p_layerMgr->create_layer(
                                         ConsoleLayer(offset_x,offset_y,width,height,this));
	stringbuf<> s("console");

	reinterpret_cast<ConsoleLayer*>(win_layer)->load_img(s.c_str());
	
}

ConsoleWindow::~ConsoleWindow()
{
	if(win_layer){
		OS.p_layerMgr->remove_layer(win_layer);
	}
}

ConsoleWindow* ConsoleWindow::CreateWindow(uint offset_x, uint offset_y, uint width, uint height)
{
	ConsoleWindow* p_win = (ConsoleWindow*)OS.p_mem_mgr->malloc(sizeof(ConsoleWindow));
	if(!p_win) return 0;

	new(p_win) ConsoleWindow((*p_win));
	p_win->_create_layer(offset_x,  offset_y,  width,  height);

	p_win->task = Task( OS.p_task_mgr, 
						OS.p_task_mgr->add_task(task_entry, OS.p_mem_mgr->malloc(8*1024) + 8*1024, (uint)p_win)
					);


	p_win->message_mgr.set_msg_task(p_win->task);
	p_win->task.active(Task_mgr::PT::normal, Task_mgr::LV::level_3);
	return p_win;

} 



void console_timeout(uint tmr)
{

	OS.timer_ctrl.set_timer(tmr,80,console_timeout);

}

void ConsoleWindow::_task_run()
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
            }
		}

	}

}


void ConsoleWindow::redraw()
{
	stringbuf<> s("console");
	reinterpret_cast<ConsoleLayer*>(win_layer)->load_img(s.c_str());
}