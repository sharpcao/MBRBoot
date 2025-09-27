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

	virtual void _create_layer(uint offset_x, uint offset_y, uint width, uint height);	
	void _set_task(const Task& t);

public:
	bool push_message(uint p1, uint p2);

	void xyprint(uint x, uint y, const char* str, Color8 col){
		if(win_layer) win_layer->xyprint(x,y,str,col);
	}
	
	Window() {};
	~Window();


	static void task_entry(uint param){
		reinterpret_cast<Window*>(param)->Run();
	}

	virtual void Run();

	void set_active() { active_window = this; }
	bool is_active() { return (active_window == this);} 
	static Window* get_active() { return active_window;}
	
	virtual void redraw() ;

	
	void InitWindow(uint offset_x, uint offset_y, uint width, uint height);
	
};

Window* Window::active_window = 0;





#endif