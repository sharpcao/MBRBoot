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
	_consoleLayer = (ConsoleLayer*)OS.p_layerMgr->create_layer(
                                         ConsoleLayer(offset_x,offset_y,width,height));
	stringbuf<> s("console");
	_consoleLayer->load_img(s.c_str());
}

ConsoleWindow::~ConsoleWindow()
{
	if(_consoleLayer){
		OS.p_layerMgr->remove_layer(_consoleLayer);
	}
}

ConsoleWindow* ConsoleWindow::CreateWindow(uint offset_x, uint offset_y, uint width, uint height)
{
	ConsoleWindow* p_win = (ConsoleWindow*)OS.p_mem_mgr->malloc(sizeof(ConsoleWindow));
	if(!p_win) return 0;

	new(p_win) ConsoleWindow((*p_win));
	p_win->_create_layer(offset_x,  offset_y,  width,  height);
	p_win->p_task = OS.p_task_mgr->add_task(task_handler, 
						OS.p_mem_mgr->malloc(8*1024) + 8*1024, (uint)p_win);
	p_win->task_event.set_msg_task(p_win->p_task,OS.p_task_mgr);
	OS.p_task_mgr->set_active(p_win->p_task, Task_mgr::PT::normal, Task_mgr::LV::level_3);

	return p_win;

} 

void ConsoleWindow::task_handler(uint param)
{
	ConsoleWindow* p_win = reinterpret_cast<ConsoleWindow*>(param);
	p_win->_task_handler();
}


void console_timeout(uint tmr)
{
	//OS.layers.p_debug->set_text("console",*OS.p_layerMgr,Color8::COL8_FFFFFF);
	OS.timer_ctrl.set_timer(tmr,80,console_timeout);

}

void ConsoleWindow::_task_handler()
{
	
	OS.timer_ctrl.add_timer(80,console_timeout, &task_event);
	uint p1,p2;
	for(;;)
	{
		io_cli();
		if(!task_event.get_message(p1,p2))
		{
			task_event.deact_msg_task();
			io_sti();
		}else{
			io_sti();
			if( p1 == EVENT::Timer){
				//_consoleLayer->set_title("OK", *OS.p_layerMgr);
				_consoleLayer->twinkle();
                OS.timer_ctrl.call_hander((uint)p2);
            }
		}

	}

}
