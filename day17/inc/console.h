#ifndef CONSOLE_H
#define CONSOLE_H

#include "winform.h"
#include "winos.h"

class ConsoleLayer : public CWindowLayer
{
private:
	//CRect _twinkle_box;

	uint _cursor_row = 0, _cursor_col = 0;
	uint _col_max() { return _client_box._w / 8; }
	uint _row_max() { return _client_box._h / 16; }
	void scroll_row(uint oy, uint x, uint y, uint w, uint h,  Color8 client_color = Color8::COL8_000000);

public:
	CRect cursor_client_box();
	void cursor_step(uint s = 1);
	void cursor_next();
	void cursor_show(bool bshow = true);
	void twinkle();	


	void cmd_enter();
	void add_char(uchar asc);


	ConsoleLayer(uint offset_x, uint offset_y, uint width, uint height, Window* wnd) :
		CWindowLayer(offset_x, offset_y, width, height, wnd)

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

	virtual void _create_layer(uint offset_x, uint offset_y, uint width, uint height);
	
public:

	ConsoleWindow() = default;
	~ConsoleWindow();

	virtual void redraw() ;
	virtual void Run();
};




#endif
