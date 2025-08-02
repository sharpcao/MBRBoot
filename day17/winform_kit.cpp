#include "inc\winform.h"
#include "inc\winos.h"


extern CWinOS OS;

void Window::_task_run()
{
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

		}

	}
}



void Window::_create_layer(uint offset_x, uint offset_y, uint width, uint height)
{
	win_layer = (CWindowLayer*)OS.p_layerMgr->create_layer(
                                         CWindowLayer(offset_x,offset_y,width,height,this));
	stringbuf<> s("Win");
	win_layer->load_img(s.c_str());
}


Window::~Window()
{
	if(win_layer){
		OS.p_layerMgr->remove_layer(win_layer);
	}
}

Window* Window::CreateWindow(uint offset_x, uint offset_y, uint width, uint height)
{
	Window* p_win = (Window*)OS.p_mem_mgr->malloc(sizeof(Window));
	if(!p_win) return 0;

	new(p_win) Window((*p_win));
	p_win->_create_layer(offset_x,  offset_y,  width,  height);

	p_win->task = Task( OS.p_task_mgr, 
						OS.p_task_mgr->add_task(task_entry, OS.p_mem_mgr->malloc(8*1024) + 8*1024, (uint)p_win)
					);


	p_win->message_mgr.set_msg_task(p_win->task);
	p_win->task.active(Task_mgr::PT::normal, Task_mgr::LV::level_3);
	return p_win;

} 

void Window::redraw() 
{
	stringbuf<> s("Win");
	win_layer->load_img(s.c_str());

}