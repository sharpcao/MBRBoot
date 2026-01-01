#include "inc\console.h"
#include "inc\os_io.h"
#include "inc\functions.h"

extern CWinOS OS;

extern void console_dump(stringbuf<>& cout_str, const Cmd_Parser& cmd_str);
extern void console_readhd(stringbuf<>& cout_str, const Cmd_Parser& cmd);
extern void console_loadhd(stringbuf<>& cout_str, const Cmd_Parser& cmd);
extern void console_run_at(stringbuf<>& cout_str, const Cmd_Parser& cmd);
extern void console_run(stringbuf<>& cout_s, const Cmd_Parser& cmd);
extern void console_mem(stringbuf<>& cout_str, const Cmd_Parser& cmd);
void ConsoleLayer::twinkle()
{
	static bool bshow = false;
	bshow = !bshow;
	if (bshow){
		cursor_show(bshow, true);
	}else{
		if(_cursor_pos != _cursor_cmd_end ){
			 cursor_show(bshow, false);
			 ((ConsoleWindow*)this_window)->repaint_cmd();

			 CRect box = cursor_box(_cursor_pos);
			 OS.p_layerMgr->update(box.to_vga_pos(_offset_x, _offset_y));
		}else{
			cursor_show(bshow, true);
		}
	}

}

void ConsoleLayer::cursor_show(bool bshow, bool update_now)
{
	Color8 color = bshow ? Color8::COL8_FFFFFF : Color8::COL8_000000;
	CRect box = cursor_box(_cursor_pos);
	fill_box(color, box);

	if(update_now) {
		OS.p_layerMgr->update(box.to_vga_pos(_offset_x, _offset_y));
	}
}

CRect ConsoleLayer::cursor_box(uint pos) const
{
	uint col = pos % _col_max();
	uint row = pos / _col_max();
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
		--row;
		scroll_row(16, _client_box._x, _client_box._y, _client_box._w, _client_box._h);
		
		_cursor_prefix_end = (_cursor_prefix_end > _col_max())? 
							(_cursor_prefix_end - _col_max()) : 0;
	}

	_cursor_pos = _cursor_cmd_end = _convert_to_pos(row, col);

}



void ConsoleLayer::cursor_nextline()
{
	uint row,col;
	_convert_from_pos(_cursor_cmd_end, row, col);

	++row;
	col = 0;

	if ( row == _row_max()){
		--row;
		scroll_row(16, _client_box._x, _client_box._y, _client_box._w, _client_box._h);

		_cursor_prefix_end = (_cursor_prefix_end > _col_max())? 
							(_cursor_prefix_end - _col_max()) : 0;
		
	}

	_cursor_pos = _cursor_cmd_end = _convert_to_pos(row,col);

}

void ConsoleLayer::append_prefix(bool newline)
{
	if (newline) cursor_nextline();
	append_string(prefix_str);
	_cursor_prefix_end = _cursor_pos;
}

void ConsoleLayer::append_string(const char* pstr)
{
	for(const char* p = pstr; *p!= 0; ++p) append_char(*p);
}

void ConsoleLayer::append_char(uchar asc)
{
	if(asc){
		cursor_show(false);

		if(asc == '\n'){
			cursor_nextline();
		}else{

			paint_char(_cursor_pos, asc);
			cursor_step();
		}
		
	}
}

void ConsoleLayer::clear_screen()
{
	fill_client_box(Color8::COL8_000000);
	_cursor_prefix_end = 0, _cursor_cmd_end = 0, _cursor_pos = 0;

}

void ConsoleWindow::cmd_enter(const stringbuf<>& cmd_str)
{
	ConsoleLayer* layer = static_cast<ConsoleLayer*> (win_layer);
	layer->cursor_show(false);
	layer->paint_cmd(cmd_str);
	layer->cursor_nextline();
	stringbuf<> cout_s;
	bool newline = true;

	Cmd_Parser cmd(cmd_str.c_str());

	if(cmd_str.size()){

		if (cmd[0] =="cls"){
			layer->clear_screen();
			newline = false;

		}else if (cmd[0] == "mem"){
			
			console_mem(cout_s, cmd);
			
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
		}else if(cmd[0] == "run"){
			console_run(cout_s, cmd);
		}else if(cmd[0]=="test"){

		}else{
			cout_s << cmd_str.c_str();
		}
		
	}
	if (cout_s.size()) layer->append_string(cout_s.c_str());
	layer->append_prefix(newline);
	
}



void ConsoleLayer::paint_char(uint pos, uchar asc)
{
	CRect box = cursor_box(pos);
	if(asc){
		putfont8(box._x, box._y, asc, COL8_FFFFFF);
	}else{
		fill_box(Color8::COL8_000000, box);
	}
}



void ConsoleLayer::remove_to_head()
{

	cursor_show(false);
	uint pos0 = _cursor_prefix_end;
	uint pos1 = _cursor_cmd_end;
	for(uint pos = pos0; pos<= pos1; ++pos)
	{
		paint_char(pos, 0);
	}
	_cursor_pos = _cursor_cmd_end = _cursor_prefix_end;

}

void ConsoleLayer::paint_cmd(const stringbuf<>& cmd_str)
{
	
	uint cmd_size = cmd_str.size();
	uint pos_end = max(_cursor_prefix_end + cmd_size, _cursor_cmd_end);
	for(uint pos = _cursor_prefix_end; pos <= pos_end; ++pos)
	{
		paint_char(pos, 0);
		uint i = pos - _cursor_prefix_end;

		if(i < cmd_size) paint_char(pos, cmd_str[i]);

		if(pos > _col_max() * _row_max()){
			scroll_row(16, _client_box._x, _client_box._y, _client_box._w, _client_box._h);
			uint ncol = _col_max();
			pos -= ncol;
			pos_end -= ncol;
			_cursor_prefix_end = (_cursor_prefix_end >= ncol)? _cursor_prefix_end - ncol : 0;
		}


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
	((ConsoleLayer*)win_layer)->paint_cmd(_cmd_str);
}


void ConsoleWindow::Run()
{
	//active();
	
	ConsoleLayer* consoleLayer =  reinterpret_cast<ConsoleLayer*>(win_layer);
	cur_ConsoleLayer = consoleLayer;
	
	consoleLayer->set_title(0, *OS.p_layerMgr);
	consoleLayer->append_prefix(false);
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
      			continue;

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
            		cmd_enter(_cmd_str);
            		_cmd_str.reset();
      				refresh_console();
            	}else if(p2 == TransKey::KeyCode::Backspace){
            		// _cmd_str.pop();
            		// consoleLayer->remove_backchar();
            		consoleLayer->cursor_show(false);
            		consoleLayer->cursor_move_left();
					_cmd_str.remove_char(consoleLayer->cursor_char_offset());
					repaint_cmd();
            		refresh_console();
            	}else if (p2 == TransKey::KeyCode::ArrowUp){
            		consoleLayer->remove_to_head();
            		_cmd_str = _cmd_history.get_prev();
            		consoleLayer->append_string(_cmd_str.c_str());
            		refresh_console();
            	}else if (p2 == TransKey::KeyCode::ArrowDown){
            		consoleLayer->remove_to_head();
            		_cmd_str = _cmd_history.get_next();
            		consoleLayer->append_string(_cmd_str.c_str());
            		refresh_console();
            	}else if (p2 == TransKey::KeyCode::ArrowLeft){
            		consoleLayer->cursor_show(false);
            		consoleLayer->cursor_move_left();
            		repaint_cmd();
            		refresh_console();
            	}else if (p2 == TransKey::KeyCode::ArrowRight){
            
            		consoleLayer->cursor_show(false);
            		consoleLayer->cursor_move_right();
            		repaint_cmd();
            		refresh_console();
            	}else{

					uchar c = OS.translate_keycode(p2);
	           		if(c) {

	           			_cmd_str.insert(consoleLayer->cursor_char_offset(), c);
	           			consoleLayer->cursor_show(false);
	           			repaint_cmd();
	           			consoleLayer->cursor_move_right();

	            	}
	            	refresh_console();
	            }	       
            }
		}
	}
}



void ConsoleWindow::redraw()
{
	reinterpret_cast<ConsoleLayer*>(win_layer)->load_img(0);
}