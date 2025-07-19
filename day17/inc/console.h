#ifndef CONSOLE_H
#define CONSOLE_H

#include "layer.h"
#include "task_kit.h"
#include "winos.h"
#include "functions.h"

class ConsoleLayer : public CWindowLayer
{
private:
	CRect _twinkle_box;
	
public:
	void twinkle();
	ConsoleLayer(uint offset_x, uint offset_y, uint width, uint height) :
		CWindowLayer(offset_x, offset_y, width, height), _twinkle_box(_client_box._x+2, _client_box._y+2, 10,18)

	{}

	void load_img(const char* title = "", Color8 client_color = Color8::COL8_000000){
		CWindowLayer::load_img(title, client_color);
	}

	virtual uint class_size  ()  const 
		{ return sizeof(ConsoleLayer);}

	virtual ConsoleLayer* class_new(char* addr) const {
		return new(addr) ConsoleLayer(*this); 
	}


};



class ConsoleWindow {
private:
	ConsoleLayer* _consoleLayer = 0;
	Task_Message_mgr message_mgr;
	Task task;
	
	void _create_layer(uint offset_x, uint offset_y, uint width, uint height);
	void _task_run();

public:
	ConsoleWindow() = default;
	~ConsoleWindow();
	void active() { _consoleLayer->set_active(); }

	static ConsoleWindow* CreateWindow(uint offset_x, uint offset_y, uint width, uint height);
	static void task_entry(uint param);

};

void ConsoleWindow::task_entry(uint param)
{
	reinterpret_cast<ConsoleWindow*>(param)->_task_run();
}


#endif
