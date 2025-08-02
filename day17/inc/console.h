#ifndef CONSOLE_H
#define CONSOLE_H

#include "winform.h"
#include "winos.h"

class ConsoleLayer : public CWindowLayer
{
private:
	CRect _twinkle_box;
	
public:
	void twinkle();
	ConsoleLayer(uint offset_x, uint offset_y, uint width, uint height, Window* wnd) :
		CWindowLayer(offset_x, offset_y, width, height, wnd), _twinkle_box(_client_box._x+2, _client_box._y+2, 10,18)

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



class ConsoleWindow : public Window {
private:
	//ConsoleLayer* _consoleLayer = 0;
	
	void _create_layer(uint offset_x, uint offset_y, uint width, uint height);
	virtual void _task_run();

public:
	ConsoleWindow() = default;
	~ConsoleWindow();

	static ConsoleWindow* CreateWindow(uint offset_x, uint offset_y, uint width, uint height);
	virtual void redraw() ;

};




#endif
