#ifndef CONSOLE_H
#define CONSOLE_H

#include "layer.h"
#include "task_kit.h"
#include "winos.h"
#include "functions.h"

class ConsoleLayer : public CWindowLayer
{
public:
	ConsoleLayer(uint offset_x, uint offset_y, uint width, uint height) :
		CWindowLayer(offset_x, offset_y, width, height) {}
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
	Task* p_task = 0;
	Task_Message_mgr task_event;
	
	void _create_layer(uint offset_x, uint offset_y, uint width, uint height);
	void _task_handler();

public:
	ConsoleWindow() = default;
	~ConsoleWindow();

	static ConsoleWindow* CreateWindow(uint offset_x, uint offset_y, uint width, uint height);
	static void task_handler(uint param);

};




#endif
