#include "inc\console.h"
#include "inc\os_io.h"
#include "inc\functions.h"

extern CWinOS OS;



void ConsoleLayer::twinkle()
{
	static bool bshow = false;

	// Color8 col = bshow ? Color8::COL8_000000 : Color8::COL8_FFFFFF;

	// CRect _twinkle_box = cursor_client_box().add_offset(_client_offset_x, _client_offset_y);
	// fill_box(col, _twinkle_box);
	bshow = !bshow;
	cursor_show(bshow);
	
	
}

void ConsoleLayer::cursor_show(bool bshow)
{
	Color8 col = bshow ? Color8::COL8_FFFFFF : Color8::COL8_000000;
	CRect _twinkle_box = cursor_client_box().add_offset(_client_offset_x, _client_offset_y);
	fill_box(col, _twinkle_box);
	OS.p_layerMgr->update(_twinkle_box.to_vga_pos(_offset_x, _offset_y));
}

CRect ConsoleLayer::cursor_client_box()
{
	uint w = _cursor_col*8 + 2;
	
	return CRect( 
			 _cursor_col*8 + 2, 
			 _cursor_row*16 + 2,
			 8, 16);
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
	uint n = _cursor_col + s;
	_cursor_col = n % _col_max();
	_cursor_row += n / _col_max();
	if (_cursor_row == _row_max()){
		_cursor_row = _row_max()-1;
		scroll_row(16, _client_box._x, _client_box._y, _client_box._w, _client_box._h);
	}
}

void ConsoleLayer::cursor_next()
{
	
	_cursor_col = 0;
	if ( ++_cursor_row == _row_max()){
		_cursor_row = _row_max()-1;
		scroll_row(16, _client_box._x, _client_box._y, _client_box._w, _client_box._h);
	}
}


void ConsoleLayer::cmd_enter(const stringbuf<>& cmd_str)
{
	cursor_show(false);
	cursor_next();
	if(cmd_str.size()){
		if (cmd_str == "mem"){
			stringbuf<> mem_str;
			mem_str <<  OS.p_mem_mgr->get_mem_total() /1024 << " KB, "
				<<"free:" << OS.p_mem_mgr->get_mem_free() /1024 << " KB\n";
			add_string(mem_str.c_str());
		}if (cmd_str== "a"){
				
	            
		}
		else{
			add_string(cmd_str.c_str());
		}
		cursor_next();	
	}
	add_string(ConsoleLayer::prefix_str);
	OS.p_layerMgr->update(get_area());
}

void ConsoleLayer::add_char(uchar asc)
{
	if(asc){
		CRect cursor_box = cursor_client_box().add_offset(_client_offset_x, _client_offset_y);
		fill_box(Color8::COL8_000000, cursor_box);
		if(asc == '\n'){
			cursor_next();
		}else{
			putfont8(cursor_box._x, cursor_box._y,asc, COL8_FFFFFF);
			cursor_step();
		}
		
	}
}
void ConsoleLayer::add_string(const char* pstr)
{
	for(const char* p = pstr; *p!= 0; ++p) add_char(*p);

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

void ConsoleWindow::Run()
{
	//active();
	ConsoleLayer* consoleLayer =  reinterpret_cast<ConsoleLayer*>(win_layer);
	
	consoleLayer->set_title(0, *OS.p_layerMgr);
	consoleLayer->add_string(ConsoleLayer::prefix_str);
	OS.p_layerMgr->update(consoleLayer->get_area());

	OS.timer_ctrl.add_timer(80,console_timeout, &message_mgr);
	uint p1,p2;
	stringbuf<> cmd_str;
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

					//static uint tmp = 0;
				consoleLayer->twinkle();
                OS.timer_ctrl.call_hander((uint)p2);
                	// if(++tmp > 50) {
                	// 	tmp = 0;
                	// 	message_mgr.push_message(EVENT::Key,3);
                	 	OS.p_task_mgr->switch_to(1);
                	// }

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
            		consoleLayer->cmd_enter(cmd_str);
            		cmd_str.reset();
            		continue;
            	}

        	  	static uint count = 0;
			    count++;
			    stringbuf<> s;
			    s << (uint)count << "s";
			    consoleLayer->add_string(s.c_str());

	            OS.p_layerMgr->update(consoleLayer->get_area());

	     	
	            OS.p_task_mgr->switch_to(0);


            }
		}

	}

}


void ConsoleWindow::redraw()
{
	reinterpret_cast<ConsoleLayer*>(win_layer)->load_img(0);
}