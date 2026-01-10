#ifndef CONSOLE_H
#define CONSOLE_H

#include "winform.h"
#include "winos.h"


class ConsoleLayer : public CWindowLayer
{
private:
	
	uint _cursor_prefix_end = 0, _cursor_cmd_end = 0, _cursor_pos = 0;
	static const char* prefix_str;	
	
	stringbuf<> _cmd_str;

	uint _col_max() const { return _client_box._w / 8; }
	uint _row_max() const { return _client_box._h / 16; }
	uint _convert_to_pos(uint row, uint col) const { return (row * _col_max() + col);}
	void _convert_from_pos(uint pos,  uint& row,  uint& col)  {
		col = pos % _col_max();
		row = pos / _col_max();
	}

	void scroll_row(uint oy, uint x, uint y, uint w, uint h,  Color8 client_color = Color8::COL8_000000);
	
	void paint_char(uint pos, uchar asc);
	CRect cursor_box(uint pos) const;

public:

	bool is_pos_end() { return (_cursor_pos == _cursor_cmd_end) ;}
	void cursor_step(uint s = 1);	
	void cursor_nextline();	
	void cursor_show(bool bshow = true, bool update_now = false);
	void paint_cmd(const stringbuf<>& cmd_str);

	uint cursor_char_offset() const { return _cursor_pos - _cursor_prefix_end;}
	void twinkle();	
	
	void append_char(uchar asc);
	void append_string(const char* pstr);
	void append_prefix(bool newline = true);

	void remove_to_head();
	void clear_screen();

	void refresh() { OS.p_layerMgr->update(get_area()); }
	void cursor_move_left() {
		if (--_cursor_pos <= _cursor_prefix_end ) _cursor_pos  =  _cursor_prefix_end;
	}

	void cursor_move_right(){
		if (++_cursor_pos > _cursor_cmd_end ) _cursor_pos  =  _cursor_cmd_end ;
	}



	ConsoleLayer(uint offset_x, uint offset_y, uint width, uint height, Window* wnd) :
		CWindowLayer(offset_x, offset_y, width, height, wnd)

	{}

	void load_img(const char* title = "", Color8 client_color = Color8::COL8_000000){
		CWindowLayer::load_img(title, client_color);
	}


	virtual uint class_size ()  const 
		{ return sizeof(ConsoleLayer);}

	virtual ConsoleLayer* class_new(char* addr) const {
		return new(addr) ConsoleLayer(*this); 
	}


};

const char* ConsoleLayer::prefix_str = "a:>";

class ConsoleWindow : public Window {
private:
	Cmd_History _cmd_history;
	stringbuf<> _cmd_str;	
	TaskItem* this_task = 0;

	virtual void _create_layer(uint offset_x, uint offset_y, uint width, uint height);

public:

	ConsoleWindow() = default;
	~ConsoleWindow();

	virtual void redraw() ;
	virtual void Run();
	virtual void terminate();

	void repaint_cmd();
	void cmd_enter(const stringbuf<>& cmd_str);
	void terminate_app();
	void refresh_console() { reinterpret_cast<ConsoleLayer*>(win_layer)->refresh(); }
};



extern void __stdcall console_print_char(uchar c);


#endif
