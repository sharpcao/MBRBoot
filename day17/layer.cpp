#include "inc\layer.h"
#include "inc\functions.h"
#include "inc\winos.h"


Layer* Layer::class_new(char* addr) const 
{ 
	return new(addr) Layer(*this) ; 
}


inline Color8 Layer::get_pos_color(uint x, uint y)
{
	return (x < _width && y < _height)? (Color8)_img_data[y * _width + x] : Color8::COL8_TP;
}

inline Color8  Layer::get_abspos_color(uint x, uint y)
{
	return (x >= _offset_x && y >= _offset_y)? get_pos_color(x-_offset_x,y-_offset_y) : Color8::COL8_TP;
}



bool Layer::set_pos_color(uint x, uint y, Color8 color)
{
	
	if ((x < _width) && (y < _height)){
		_img_data[y * _width + x] = color;
		return true;
	}else{
		return false;
	}
}

void Layer::putfont8(uint x, uint y, uchar asc, Color8 col)
{
	static uchar* _pfont = CVGA::get_font();
	char* pos_limit = _img_data + img_size();
	uchar* asc_data = _pfont+ asc * 16;
    for(uint i =0; i < 16; ++i)
    {
        char* pos = _img_data + (y + i) * _width + x;
        char d = asc_data[i];
      
        // for(uint j=0; j < 8 ; ++j)
        // {
        //     char b = (0b10000000 >> j);
        //     if((asc_data[i] & b) && (pos+j < pos_limit) ) { pos[j] = col;}

        // }
        if (pos+7 >= pos_limit) break;
        if ((d & 0x80) != 0) { pos[0] = col; }
		if ((d & 0x40) != 0) { pos[1] = col; }
		if ((d & 0x20) != 0) { pos[2] = col; }
		if ((d & 0x10) != 0) { pos[3] = col; }
		if ((d & 0x08) != 0) { pos[4] = col; }
		if ((d & 0x04) != 0) { pos[5] = col; }
		if ((d & 0x02) != 0) { pos[6] = col; }
		if ((d & 0x01) != 0) { pos[7] = col; }

    }
}

void Layer::xyprint(uint x, uint y, const char* str, Color8 col)
{
    for(;*str != 0; ++str, x+=8)
        putfont8(x,y,*str,col);

}


Layer* Layer_mgr::create_layer(const Layer& ly)
{
	if(_end >= _MAX_LAYERS) return 0;

	Layer* p_layer = (Layer*)_mmgr.malloc(ly.class_size());
	if (p_layer == 0) return 0;


	_layers[_end] = ly.class_new((char*)p_layer);

	uint amem = _mmgr.malloc(ly._width*ly._height);
	if(amem){
		_layers[_end]->set_mem((char*)amem);
		return _layers[_end++];
	}else{
		_mmgr.free((uint)_layers[_end],ly.class_size());
	}

	return 0;
}

void Layer::draw_rect(uint x, uint y,uint width, uint height, Color8 color, char* buf)
{
	for (uint i = 0; i < height; ++i)
    {
        if (y + i >= _height) break;
        for(uint j = 0; j < width; ++j)
        {
            if(x + j >= _width) break;
            _img_data[(y+i)*_width + x + j] = (buf)? buf[i*width + j] : color;
        }
    }
}

inline void Layer::fill_box(Color8 color,uint x0, uint y0, uint x1, uint y1)
{
	draw_rect(x0,y0,x1-x0+1,y1-y0+1,color);
}

void Layer::fill_box(Color8 color,  CRect& rect)
{
	fill_box(color, rect._x,rect._y, rect._x + rect._w, rect._y+rect._h);
}

bool Layer_mgr::remove_layer(Layer* p_layer)
{
	for(uint i = 0; i <_end ;++i)
	{
		if(_layers[i]==p_layer){
			_mmgr.free((uint)_layers[i]->get_mem(),_layers[i]->img_size());
			_mmgr.free((uint)_layers[i],sizeof(Layer));
			for(uint j = i; j < _end -1; ++j) _layers[j] = _layers[j+1];
			return true;
		}
	}
	return false;
}

void Layer_mgr::print()
{

DEBUG_BUILD(
	for(uint i = 0 ;i < _end; ++i){
		std::cout << i << " addr:" << _layers[i] << " width:"<<_layers[i]->width() 
		<< " height:" << _layers[i]->height() << " offset_x:" << _layers[i]->_offset_x
		<< " offset_y:" << _layers[i]->_offset_y << std::endl;

	}
)

}


char* Layer_mgr::update(uint x0,uint y0, uint w, uint h)
{
	for(uint z = 1; z<_end ;++z){
		auto layer = _layers[z];
		if (!layer->_visible) continue;
		uint x_s = max(layer->_offset_x,x0);
		uint x_lim = min(layer->_offset_x + layer->_width, min(x0 + w, _MAX_WIDTH));
		uint y_s = max(layer->_offset_y, y0);
		uint y_lim = min(layer->_offset_y + layer->_height, min(y0 + h, _MAX_HEIGHT));
		for(uint y = y_s; y < y_lim ;++y)
		{
			for(uint x = x_s; x < x_lim; ++x){
				Color8 color = layer->get_abspos_color(x,y);
				if (color !=Color8::COL8_TP) _layers[0]->set_pos_color(x,y,color);

			}
		}

	}
	updateEvent(CRect(x0,y0,w,h));
	return (char*)_layers[0]->get_mem();
}

void CDesktopLayer::load_img()
{

	fill_box(Color8::COL8_008484, 0, 0, _width - 1, _height - 29);
    fill_box(Color8::COL8_E6E6E6, 0, _height - 28, _width - 1, _height - 28);
    fill_box(Color8::COL8_FFFFFF, 0, _height - 27, _width - 1, _height - 27);
    fill_box(Color8::COL8_E6E6E6, 0, _height - 26, _width - 1, _height - 1);

    fill_box(Color8::COL8_FFFFFF, 3, _height - 24, 59, _height - 24);
    fill_box(Color8::COL8_FFFFFF, 2, _height - 24, 2, _height - 4);
    fill_box(Color8::COL8_848484, 3, _height - 4, 59, _height - 4);
    fill_box(Color8::COL8_848484, 59, _height - 23, 59, _height - 5);
    fill_box(Color8::COL8_000000, 2, _height - 3, 59, _height - 3);
    fill_box(Color8::COL8_000000, 60, _height - 24, 60, _height - 3);
    
    fill_box(Color8::COL8_848484, _width - 47, _height - 24, _width - 4, _height - 24);
    fill_box(Color8::COL8_848484, _width - 47, _height - 23, _width - 47, _height - 4);
    fill_box(Color8::COL8_FFFFFF, _width - 47, _height - 3, _width - 4, _height - 3);
    fill_box(Color8::COL8_FFFFFF, _width - 3, _height - 24, _width - 3, _height - 3);
    xyprint(10,_height -22,"Start",Color8::COL8_848484);
}

void CWindowLayer::_draw_button()
{
	extern const char* RES_BTN_CLOSE[14];
	
	for(uint y = 0; y<14;++y){
		for(uint x = 0; x<16;++x){
			char c = RES_BTN_CLOSE[y][x];
			Color8 color = Color8::COL8_FFFFFF;
			if (c == '@'){
				color = Color8::COL8_000000;
			}else if( c== '$'){
				color = Color8::COL8_848484;
			}else if (c == 'Q'){
				color = Color8::COL8_E6E6E6;
			}
			set_pos_color(_width - 21 + x, y+5,color);
		}
	}
}

void CWindowLayer::load_img(const char* title, Color8 client_color)
{

	//顶部二横
	fill_box(Color8::COL8_E6E6E6, 0, 0, _width - 1, 0 );
	fill_box(Color8::COL8_FFFFFF, 1, 1, _width - 2, 1 );

	//左边二竖
	fill_box(Color8::COL8_E6E6E6, 0, 0, 0, _height - 1);
	fill_box(Color8::COL8_FFFFFF, 1, 1, 1, _height - 2);

	//右边二竖
	fill_box(Color8::COL8_848484, _width - 2, 1, _width - 2, _height - 2);
	fill_box(Color8::COL8_000000, _width - 1, 0, _width - 1, _height - 1);


	//底边二横
	fill_box(Color8::COL8_848484, 1, _height - 2, _width - 2, _height - 2);
	fill_box(Color8::COL8_000000, 0, _height - 1, _width - 1, _height - 1);

	//标题栏
	fill_title_box();

	//客户区
	//fill_box(client_color, 2, 23, _width - 3, _height - 3);
	fill_box(client_color, _client_box._x, _client_box._y, _client_box._w, _client_box._h);


	set_title(title);

}

void CWindowLayer::fill_title_box()
{

	fill_box(Color8::COL8_E6E6E6,2,2,_width -3,22);
	//fill_box(Color8::COL8_000084, 3, 3, _width - 4, 20 );

	Color8 background = (this_window && this_window->is_active()) ? Color8::COL8_000084 : Color8::COL8_848484; 

	fill_box(background, _title_box._x, _title_box._y, _title_box._w, _title_box._h);
	_draw_button();

}
void CWindowLayer::set_title(const char* title, Color8 font_color)
{
	if(title) _title = title;
	fill_title_box();
	xyprint(_title_box._x+2, _title_box._y+2, _title.c_str(), font_color);

}

void CWindowLayer::set_title(const char* title, Layer_mgr& lymgr, Color8 font_color)
{
	set_title(title, font_color);
	lymgr.update(_title_box.to_vga_pos(_offset_x,_offset_y));

}


void CTextLayer::set_text(const char* p_text, Color8 font_color)
{
	fill(Color8::COL8_TP);
	xyprint(0,0,p_text,font_color);

}
void CTextLayer::set_text(const char* p_text, Layer_mgr& lymgr, Color8 font_color)
{
	set_text(p_text, font_color);
	lymgr.update(get_area());
}


void CInputLayer::add_key(uint keycode,Layer_mgr& lymgr)
{
	extern CWinOS OS;
	extern char keytable[];	
	stringbuf<> s;

	
	if(keycode < 0x54) {
		char c = keytable[keycode];
		s << _cursor_x << " " << _cursor_y << " " << c;
		
		if (_cursor_x < _width){
			OS.debug_print(s.c_str());
			putfont8(_cursor_x,_cursor_y,c,Color8::COL8_000000);
			//lymgr.update(_cursor_x,_cursor_y,8,20);

			lymgr.update(get_area());
			_cursor_x+=8;
		}
	}
}



void Layer_mgr::move_layer_by(uint id, int dx, int dy)
{
	if (id > 0 && id < _end) {
		_layers[id]->_offset_x = set_inrange<int>(_layers[id]->_offset_x + dx, 0, _MAX_WIDTH);
		_layers[id]->_offset_y = set_inrange<int>(_layers[id]->_offset_y + dy, 0, _MAX_HEIGHT);
	}
}

void Layer_mgr::set_to_top(uint layer_idx)
{
	if (layer_idx >=_bottom_idx && layer_idx < _end - 1){
		Layer* ly = _layers[layer_idx];
		for(uint i = layer_idx; i < _end - 1; ++i) _layers[i] = _layers[i+1];
		_layers[_end - 1] = ly;
	}
}

void Layer_mgr::set_to_bottom(uint layer_idx)
{
	if (layer_idx >_bottom_idx && layer_idx <= _end-1){
		Layer* ly = _layers[layer_idx];
		for(uint i = layer_idx; i > _bottom_idx; --i) _layers[i] = _layers[i-1];
		_layers[_bottom_idx] = ly;
	}
}
void Layer_mgr::set_to_top(const Layer& ly)
{
	set_to_top(get_layer_idx(ly));
}

void Layer_mgr::set_to_bottom(const Layer& ly)
{
	set_to_bottom(get_layer_idx(ly));
}

uint Layer_mgr::get_layer_idx(const Layer& ly)
{
	uint i = _end;
	for(; i > 0 ; --i) if (_layers[i] == &ly) break;
	return i; 
}
