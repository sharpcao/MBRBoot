#include "inc\console.h"
#include "inc\os_io.h"
#include "inc\functions.h"

extern CWinOS OS;

extern void console_dump(stringbuf<>& cout_str, const Cmd_Parser& cmd_str);
extern void console_readhd(stringbuf<>& cout_str, const Cmd_Parser& cmd);
extern void console_loadhd(stringbuf<>& cout_str, const Cmd_Parser& cmd);
extern void console_run_at(stringbuf<>& cout_str, const Cmd_Parser& cmd);


void ConsoleLayer::twinkle()
{
	static bool bshow = false;
	bshow = !bshow;
	cursor_show(bshow);
	
}

void ConsoleLayer::cursor_show(bool bshow)
{
	Color8 col = bshow ? Color8::COL8_FFFFFF : Color8::COL8_000000;
	CRect _twinkle_box = client_box(_cursor_pos);
	fill_box(col, _twinkle_box);

	if(bshow == false) ((ConsoleWindow*)this_window)->repaint_cmd();

	OS.p_layerMgr->update(_twinkle_box.to_vga_pos(_offset_x, _offset_y));
}

CRect ConsoleLayer::client_box(uint pos) 
{
	uint row, col;
	_convert_from_pos(pos, row, col);
	return CRect(col*8 + 2, row*16 + 2,8, 16).add_offset(_client_offset_x, _client_offset_y);
}

void ConsoleLayer::scroll_row(uint oy, uint x, uint y, uint w, uint h, Color8 client_color )
{
	if (x + w > _width) w = _width - x;
	if (y + h > _height) h = _height - y;

	uint i;
	for(i = y; i < y + h - oy; ++i)
	{
		for(uint j = x; j < x + w; ++j)
		{
			_img_data[i*_width + j] = _img_data[(i+oy)*_width + j];
		}
		
	}
	for(; i < y + h; ++i)
	{
		for(uint j = x; j < x + w; ++j)
		{
			_img_data[i*_width + j] = client_color;
		}
		
	}
}

void ConsoleLayer::cursor_step(uint s)
{
	uint row,col;
	_convert_from_pos(_cursor_cmd_end, row, col);
	uint n = col + s;

	col = n % _col_max();
	row += n / _col_max();

	if (row == _row_max()){
		row = _row_max() - 1;
		scroll_row(16, _client_box._x, _client_box._y, _client_box._w, _client_box._h);
		
		int prefix_end = _cursor_prefix_end - _row_max();
		_cursor_prefix_end = (prefix_end > 0)? prefix_end:0;
	}
	_cursor_pos = _cursor_cmd_end = _convert_to_pos(row, col);

}

void ConsoleLayer::cursor_back(uint s)
{

	_cursor_pos = _cursor_cmd_end - s;
	if (_cursor_pos < _cursor_prefix_end) _cursor_pos = _cursor_prefix_end;
	_cursor_cmd_end = _cursor_pos;
}

void ConsoleLayer::cursor_next()
{
	uint row,col;
	_convert_from_pos(_cursor_cmd_end, row, col);
	col = 0;

	if ( ++row == _row_max()){
		row = _row_max()-1;
		scroll_row(16, _client_box._x, _client_box._y, _client_box._w, _client_box._h);

		int prefix_end = _cursor_prefix_end - _row_max();
		_cursor_prefix_end = (prefix_end > 0)? prefix_end:0;
		
	}

	_cursor_pos = _convert_to_pos(row,col);
	_cursor_cmd_end = _cursor_pos;

}

void ConsoleLayer::add_prefix(bool newline)
{
	if (newline) cursor_next();
	add_string(prefix_str);
	_cursor_prefix_end = _cursor_pos;
}

void ConsoleLayer::add_string(const char* pstr)
{
	for(const char* p = pstr; *p!= 0; ++p) add_char(*p);
}

void ConsoleLayer::add_char(uchar asc)
{
	if(asc){
		CRect cursor_box = client_box(_cursor_pos);
		fill_box(Color8::COL8_000000, cursor_box);
		if(asc == '\n'){
			cursor_next();
		}else{
			putfont8(cursor_box._x, cursor_box._y,asc, COL8_FFFFFF);
			cursor_step();
		}
		
	}
}


void ConsoleLayer::cmd_enter(const stringbuf<>& cmd_str)
{
	cursor_show(false);
	cursor_next();
	stringbuf<> cout_s;
	bool newline = true;

	Cmd_Parser cmd(cmd_str.c_str());

	if(cmd_str.size()){

		if (cmd[0] =="cls"){
			fill_client_box(Color8::COL8_000000);
			_cursor_prefix_end = 0, _cursor_cmd_end = 0, _cursor_pos = 0;

			newline = false;

		}else if (cmd[0] == "mem"){
			
			cout_s <<  OS.p_mem_mgr->get_mem_total() /1024 << " KB, "
				<<"free:" << OS.p_mem_mgr->get_mem_free() /1024 << " KB\n";
			
		}else if( cmd[0] == "ver"){
			cout_s <<"VOS 0.0.1";

		}else if(cmd[0] == "dump"){
			console_dump(cout_s, cmd);

		}else if(cmd[0] == "readhd"){
			console_readhd(cout_s, cmd);
		}else if(cmd[0] == "loadhd"){
			console_loadhd(cout_s, cmd);
		}
		else if (cmd[0] == "?" || cmd[0] == "help"){
			cout_s <<"cmd: cls mem ver dump readhd loadhd runat\n";
		
		}else if(cmd[0] == "runat"){
			console_run_at(cout_s, cmd);
		}else if(cmd[0]=="test"){
			OS.p_mem_mgr_big->print(cout_s);

		}else{
			cout_s << cmd_str.c_str();
		}
		
	}
	if (cout_s.size()) add_string(cout_s.c_str());
	add_prefix(newline);
	OS.p_layerMgr->update(get_area());
}



void ConsoleLayer::paint_char(uint pos, uchar asc)
{
	CRect box = _get_rect_from_pos(pos);
	if(asc){
		putfont8(box._x, box._y, asc, COL8_FFFFFF);
	}else{
		fill_box(Color8::COL8_000000, box);
	}
}

void ConsoleLayer::remove_char(uint nchar)
{
	while(nchar-- ){
		// CRect cursor_box = cursor_client_box().add_offset(_client_offset_x, _client_offset_y);
		// fill_box(Color8::COL8_000000, cursor_box);
		//uint pos = _convert_to_pos(_cursor_row, _cursor_col);
		cursor_show(false);
		cursor_back();	
	}
	cursor_show(false);
}

void ConsoleLayer::remove_to_head()
{

	// int nchar = (int)_convert_to_pos(_cursor_row, _cursor_col) - _cursor_prefix_end;
	// if (nchar > 0) remove_char(nchar);
	cursor_show(false);
	uint pos0 = _cursor_prefix_end;
	uint pos1 = _cursor_cmd_end;
	for(uint pos = pos0; pos<= pos1; ++pos)
	{
		paint_char(pos, 0);
	}
	_cursor_pos = _cursor_cmd_end = _cursor_prefix_end;

}

void ConsoleLayer::paint_cmd_str(const stringbuf<>& cmd_str)
{
	//cursor_show(false);
	uint pos0 = _cursor_prefix_end;
	uint pos1 = _cursor_cmd_end;
	uint i = 0, cmd_size = cmd_str.size();
	for(uint pos = pos0; pos<= pos1; ++pos)
	{
		char c =  (i < cmd_size) ? cmd_str[i] : 0;
		if (c) paint_char(pos, 0);

		paint_char(pos, c);
		++i;
	}
	_cursor_cmd_end = _cursor_prefix_end + cmd_size;
}



void ConsoleWindow::_create_layer(uint offset_x, uint offset_y, uint width, uint height)
{
	win_layer = (ConsoleLayer*)OS.p_layerMgr->create_layer(
                                         ConsoleLayer(offset_x,offset_y,width,height,this));
	stringbuf<> s("console1");

	static_cast<ConsoleLayer*>(win_layer)->load_img(s.c_str());
	
}

ConsoleWindow::~ConsoleWindow()
{
	if(win_layer){
		OS.p_layerMgr->remove_layer(win_layer);
	}
}




void console_timeout(uint tmr)
{

	OS.timer_ctrl.set_timer(tmr,80,console_timeout);

}


extern void get_task_str(uint idx, stringbuf<>& str);

extern ConsoleLayer* cur_ConsoleLayer;

void ConsoleWindow::repaint_cmd()
{
	((ConsoleLayer*)win_layer)->paint_cmd_str(_cmd_str);
}


void ConsoleWindow::Run()
{
	//active();
	
	ConsoleLayer* consoleLayer =  reinterpret_cast<ConsoleLayer*>(win_layer);
	cur_ConsoleLayer = consoleLayer;
	
	consoleLayer->set_title(0, *OS.p_layerMgr);
	consoleLayer->add_prefix(false);
	OS.p_layerMgr->update(consoleLayer->get_area());

	OS.timer_ctrl.add_timer(80,console_timeout, &message_mgr);
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
			if( p1 == EVENT::Timer){

				consoleLayer->twinkle();
                OS.timer_ctrl.call_hander((uint)p2);
      

            }else if(p1 == EVENT::Actived){
            	//active();	
            }else if(p1 == EVENT::Key){

            	if(p2 == TransKey::KeyCode::LShiftDown) {
            		TransKey::set_lshift(true);
            		continue;
            	}else if( p2 == TransKey::KeyCode::LShiftUp){
            		TransKey::set_lshift(false);
            		continue;
            	}else if(p2 == TransKey::KeyCode::RShiftDown){
            		TransKey::set_rshift(true);
            		continue;
            	}else if(p2 == TransKey::KeyCode::RShiftUp){
            		TransKey::set_rshift(false);
            		continue;
            	}else if(p2 == TransKey::KeyCode::CapsDown){
            		TransKey::set_caps();
            		continue;
            	}else if(p2 == TransKey::KeyCode::EnterDown){
            		if (!_cmd_str.is_empty()) _cmd_history.add_cmd(_cmd_str);
            		consoleLayer->cmd_enter(_cmd_str);
            		_cmd_str.reset();
            		continue;
            	}else if(p2 == TransKey::KeyCode::Backspace){
            		_cmd_str.pop();
            		consoleLayer->remove_char();
            		//continue;
            	}else if (p2 == TransKey::KeyCode::ArrowUp){
            		consoleLayer->remove_to_head();
            		_cmd_str = _cmd_history.get_prev();
            		consoleLayer->add_string(_cmd_str.c_str());

            	}else if (p2 == TransKey::KeyCode::ArrowDown){
            		consoleLayer->remove_to_head();
            		_cmd_str = _cmd_history.get_next();
            		consoleLayer->add_string(_cmd_str.c_str());
            	}else if (p2 == TransKey::KeyCode::ArrowLeft){
            		consoleLayer->cursor_show(false);
            		consoleLayer->cursor_move_left();
            		repaint_cmd();
            	}else if (p2 == TransKey::KeyCode::ArrowRight){
            		consoleLayer->cursor_show(false);
            		consoleLayer->cursor_move_right();
            		repaint_cmd();
            	}else{

					uchar c = OS.translate_keycode(p2);
	           		if(c) {
	           			_cmd_str << c;
	           			//if(_cmd_str.insert(_cmd_str_idx, c)) ++_cmd_str_idx;
	           			consoleLayer->add_char(c);
	            	
	            	}
	            }
	       
	            consoleLayer->refresh();


            }
		}

	}

}



void ConsoleWindow::redraw()
{
	reinterpret_cast<ConsoleLayer*>(win_layer)->load_img(0);
}