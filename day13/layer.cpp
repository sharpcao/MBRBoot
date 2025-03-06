#include "inc\layer.h"
#include "inc\functions.h"


CLayer* CLayer::class_new_at(char* addr) const 
{ 
	return new(addr) CLayer(*this) ; 
}


inline Color8 CLayer::get_pos_color(uint x, uint y)
{
	return (x < _width && y < _height)? (Color8)_img_data[y * _width + x] : Color8::COL8_TP;
}

inline Color8  CLayer::get_abspos_color(uint x, uint y)
{
	return (x >= _offset_x && y >= _offset_y)? get_pos_color(x-_offset_x,y-_offset_y) : Color8::COL8_TP;
}



bool CLayer::set_pos_color(uint x, uint y, Color8 color)
{
	
	if ((x < _width) && (y < _height)){
		_img_data[y * _width + x] = color;
		return true;
	}else{
		return false;
	}
}

void CLayer::putfont8(uint x, uint y, uchar asc, Color8 col)
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

void CLayer::xyprint(uint x, uint y, const char* str, Color8 col)
{
    for(;*str != 0; ++str, x+=8)
        putfont8(x,y,*str,col);

}


CLayer* CLayer_Mgr::add_layer(const CLayer& ly)
{
	if(_last >= _max_layers) return 0;

	CLayer* nly = (CLayer*)_mmgr.malloc(ly.class_size());
	if (nly == 0) return 0;

	_layers[_last] = ly.class_new_at((char*)nly);

	uint amem = _mmgr.malloc(ly._width*ly._height);
	if(amem){
		_layers[_last]->set_mem((char*)amem);
		return _layers[_last++];
	}else{
		_mmgr.free((uint)_layers[_last],ly.class_size());
	}

	return 0;
}

void CLayer::draw_rect(uint x, uint y,uint width, uint height, Color8 color, char* buf)
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

inline void CLayer::fill_box(Color8 color,uint x0, uint y0, uint x1, uint y1)
{
	draw_rect(x0,y0,x1-x0+1,y1-y0+1,color);
}

bool CLayer_Mgr::remove_layer(CLayer* alayer){
	for(uint i = 0; i <_last ;++i)
	{
		if(_layers[i]==alayer){
			_mmgr.free((uint)_layers[i]->get_mem(),_layers[i]->img_size());
			_mmgr.free((uint)_layers[i],sizeof(CLayer));
			for(uint j = i; j < _last -1; ++j) _layers[j] = _layers[j+1];
			return true;
		}
	}
	return false;
}

void CLayer_Mgr::print()
{

DEBUG_BUILD(
	for(uint i = 0 ;i < _last; ++i){
		std::cout << i << " addr:" << _layers[i] << " width:"<<_layers[i]->width() 
		<< " height:" << _layers[i]->height() << " offset_x:" << _layers[i]->_offset_x
		<< " offset_y:" << _layers[i]->_offset_y << std::endl;

	}
)

}


char* CLayer_Mgr::update(uint x0,uint y0, uint w, uint h)
{
	for(uint z = 1; z<_last ;++z){
		auto layer = _layers[z];
		if (!layer->_visible) continue;
		uint x_s = max(layer->_offset_x,x0);
		uint x_lim = min(layer->_offset_x + layer->_width, min(x0 + w, _max_width));
		uint y_s = max(layer->_offset_y, y0);
		uint y_lim = min(layer->_offset_y + layer->_height, min(y0 + h, _max_height));
		for(uint y = y_s; y < y_lim ;++y)
		{
			for(uint x = x_s; x < x_lim; ++x){
				Color8 color = layer->get_abspos_color(x,y);
				if (color !=Color8::COL8_TP) _layers[0]->set_pos_color(x,y,color);

			}
		}

	}

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

void CWindowLayer::load_img(const char* title)
{

	fill_box(Color8::COL8_E6E6E6, 0, 0, _width - 1, 0 );
	fill_box(Color8::COL8_FFFFFF, 1, 1, _width - 2, 1 );
	fill_box(Color8::COL8_E6E6E6, 0, 0, 0, _height - 1);
	fill_box(Color8::COL8_FFFFFF, 1, 1, 1, _height - 2);
	fill_box(Color8::COL8_848484, _width - 2, 1, _width - 2, _height - 2);
	fill_box(Color8::COL8_000000, _width - 1, 0, _width - 1, _height - 1);
	fill_box(Color8::COL8_E6E6E6, 2, 2, _width - 3, _height - 3);
	fill_box(Color8::COL8_000084, 3, 3, _width - 4, 20 );
	fill_box(Color8::COL8_848484, 1, _height - 2, _width - 2, _height - 2);
	fill_box(Color8::COL8_000000, 0, _height - 1, _width - 1, _height - 1);

	_title = title;
	xyprint(5,5,_title.c_str(),Color8::COL8_FFFFFF);

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


void CTextLayer::set_text(const char* p_text, Color8 font_color)
{
	fill(Color8::COL8_TP);
	xyprint(0,0,p_text,font_color);
}


void CLayer_Mgr::move_layer_by(uint id, int dx, int dy)
{
	if (id > 0 && id < _last) {
		_layers[id]->_offset_x = set_inrange<int>(_layers[id]->_offset_x + dx, 0, _max_width);
		_layers[id]->_offset_y = set_inrange<int>(_layers[id]->_offset_y + dy, 0, _max_height);
	}
}

void CLayer_Mgr::set_to_top(uint layer_idx)
{
	if (layer_idx >=_bottom_idx && layer_idx < _last - 1){
		CLayer* ly = _layers[layer_idx];
		for(uint i = layer_idx; i < _last - 1; ++i) _layers[i] = _layers[i+1];
		_layers[_last - 1] = ly;
	}
}

void CLayer_Mgr::set_to_bottom(uint layer_idx)
{
	if (layer_idx >_bottom_idx && layer_idx <= _last-1){
		CLayer* ly = _layers[layer_idx];
		for(uint i = layer_idx; i > _bottom_idx; --i) _layers[i] = _layers[i-1];
		_layers[_bottom_idx] = ly;
	}
}
void CLayer_Mgr::set_to_top(const CLayer& ly)
{
	set_to_top(get_layer_idx(ly));
}

void CLayer_Mgr::set_to_bottom(const CLayer& ly)
{
	set_to_bottom(get_layer_idx(ly));
}

uint CLayer_Mgr::get_layer_idx(const CLayer& ly)
{
	uint i = _last;
	for(; i > 0 ; --i) if (_layers[i] == &ly) break;
	return i; 
}
