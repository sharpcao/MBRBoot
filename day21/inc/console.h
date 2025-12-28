#ifndef CONSOLE_H
#define CONSOLE_H

#include "winform.h"
#include "winos.h"


class ConsoleLayer : public CWindowLayer
{
private:
	
	//uint _cursor_row = 0, _cursor_col = 0;
	uint _cursor_prefix_end = 0, _cursor_cmd_end = 0, _cursor_pos = 0;

	uint _col_max() const { return _client_box._w / 8; }
	uint _row_max() const { return _client_box._h / 16; }
	uint _convert_to_pos(uint row, uint col) const { return (row * _col_max() + col);}
	void _convert_from_pos(uint pos,  uint& row,  uint& col)  {
		col = pos % _col_max();
		row = pos / _col_max();
	}
	CRect _get_rect_from_pos(uint pos) const {
		uint col = pos % _col_max();
		uint row = pos / _col_max();
		return CRect(col * 8 + 2, row * 16 + 2, 8, 16).add_offset(_client_offset_x, _client_offset_y);
	}

	void scroll_row(uint oy, uint x, uint y, uint w, uint h,  Color8 client_color = Color8::COL8_000000);
	
	static const char* prefix_str;
	void cursor_back(uint s=1);
	void paint_char(uint pos, uchar asc);

public:
	void paint_cmd_str(const stringbuf<>& cmd_str);
	CRect client_box(uint pos) ;
	
	void cursor_next();
	void cursor_show(bool bshow = true);
	void twinkle();	
	void refresh() { OS.p_layerMgr->update(get_area()); }

	void cmd_enter(const stringbuf<>& cmd_str);
	void add_char(uchar asc);
	void add_string(const char* pstr);
	void add_prefix(bool newline = true);
	void remove_char(uint nchar = 1);
	void remove_to_head();


	void cursor_move_left() {
		if (--_cursor_pos <= _cursor_prefix_end ) _cursor_pos  =  _cursor_prefix_end;
	}

	void cursor_move_right(){
		if (++_cursor_pos > _cursor_cmd_end ) _cursor_pos  =  _cursor_cmd_end ;
	}

	void cursor_step(uint s = 1);


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

const char* ConsoleLayer::prefix_str = "a:>";

class ConsoleWindow : public Window {
private:
	Cmd_History _cmd_history;
	virtual void _create_layer(uint offset_x, uint offset_y, uint width, uint height);

	stringbuf<> _cmd_str;
	uint _cmd_str_idx = 0;
	
public:

	ConsoleWindow() = default;
	~ConsoleWindow();

	virtual void redraw() ;
	virtual void Run();
	void repaint_cmd();
};



extern void __stdcall console_print_char(uchar c);


#endif
