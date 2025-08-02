#ifndef WINFORM_H
#define WINFORM_H

#include "layer.h"
#include "task_kit.h"
#include "functions.h"


class Window
{
protected:
	Task_Message_mgr message_mgr;
	Task task;
	CWindowLayer* win_layer = 0;

	static Window* active_window;
public:
	bool push_message(uint p1, uint p2){
		io_cli();
		bool result = message_mgr.push_message(p1,p2);
		io_sti();
		return result;
	}
	void default_message_handler(uint p1, uint p2) {};
	void xyprint(uint x, uint y, const char* str, Color8 col){
		if(win_layer) win_layer->xyprint(x,y,str,col);
	}
	
	Window() = default;
	~Window();


	static void task_entry(uint param){
		reinterpret_cast<Window*>(param)->_task_run();
	}

	static Window* CreateWindow(uint offset_x, uint offset_y, uint width, uint height);


	
	void set_active() { active_window = this; }

	bool is_active() { return (active_window == this);} 

	virtual void _task_run();
	virtual void redraw() ;
	virtual void _create_layer(uint offset_x, uint offset_y, uint width, uint height);

};

Window* Window::active_window = 0;





#endif