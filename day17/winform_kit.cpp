#include "inc\winform.h"
#include "inc\winos.h"


extern CWinOS OS;

bool Window::push_message(uint p1, uint p2)
{
	io_cli();
	bool result = message_mgr.push_message(p1,p2);
	io_sti();
	return result;
}

void Window::Run()
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

void Window::_set_task(const Task& t) 
{
	task = t;
	message_mgr.set_msg_task(task);
	task.active(Task_mgr::PT::normal, Task_mgr::LV::level_3);

}

Window::~Window()
{
	if(win_layer){
		OS.p_layerMgr->remove_layer(win_layer);
	}
}



void Window::redraw() 
{
	win_layer->load_img(0);

}

void Window::InitWindow(uint offset_x, uint offset_y, uint width, uint height)
{
	_create_layer(offset_x, offset_y, width, height);

	_set_task(Task( OS.p_task_mgr, 
                        OS.p_task_mgr->add_task(Window::task_entry, OS.p_mem_mgr->malloc(8*1024) + 8*1024, (uint)this)
                    ));

}