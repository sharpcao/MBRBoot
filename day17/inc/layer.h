#ifndef LAYER_H
#define LAYER_H

#include "common.h"
#include "vga.h"
#include "memory_kit.h"
#include "functions.h"


class Layer_mgr;

class Layer{
protected:
	uint _offset_x = 0, _offset_y = 0;
	uint _width = 0, _height = 0;
	bool _visible = true;
	char* _img_data = 0;  // _width * _height 
public:
	Layer(uint offset_x, uint offset_y, uint width, uint height)
			:_offset_x(offset_x), _offset_y(offset_y),_width(width),_height(height)
			{}
	Layer(const Layer&) = default;

	void load_img( Color8 color = Color8::COL8_TP ) {if(_img_data) fill(color);}

	bool set_pos_color(uint x, uint y, Color8 color);
	void fill(Color8 color){
			for(uint i = 0; i<_width * _height;++i ) _img_data[i]=color;
		}
	Color8  get_pos_color(uint x, uint y);
	Color8  get_abspos_color(uint x, uint y);

	void putfont8(uint x, uint y, uchar asc, Color8 col = Color8::COL8_FFFFFF);
	void xyprint(uint x, uint y, const char* str, Color8 col= Color8::COL8_FFFFFF);

	void hide() { _visible =false; }
	void show() { _visible =true; }
	uint width() { return _width; }
	uint height() { return _height; }
	uint img_size() { return _width*_height; }
	char* get_mem() { return _img_data; }
	void set_mem(char* addr) { _img_data = addr; }
	void draw_rect(uint x, uint y,uint width, uint height, Color8 color, char* buf =0);
	void fill_box(Color8 color, uint x0, uint y0, uint x1, uint y1);
	void fill_box(Color8 color,  CRect& rect);
	CRect get_area() const { return CRect(_offset_x,_offset_y,_width,_height);}

	virtual uint class_size() const  { return sizeof(Layer);}
	virtual Layer* class_new(char* addr) const ;
	
	friend class Layer_mgr;

};


class CDesktopLayer: public Layer
{
public:
	CDesktopLayer(uint scrnx, uint scrny): Layer(0,0,scrnx,scrny) {}
	void load_img();
	virtual uint class_size ()  const
		{ return sizeof(CDesktopLayer);}
	virtual CDesktopLayer* class_new(char* addr) const {
		return new(addr) CDesktopLayer(*this); 
	}
		
};


class Window;

class CWindowLayer : public Layer
{
protected:
	stringbuf<> _title;
	CRect _title_box, _client_box;	
	Window* this_window;

	void _draw_button();
public:
	CWindowLayer(uint offset_x, uint offset_y, uint width, uint height, Window* wnd)
						: Layer(offset_x,offset_y, width, height),
						 _title_box(3,3,_width - 4, 20), _client_box(2,23,_width -3, _height -3),
						 this_window(wnd)
	{}

	void set_title(const char* title, Color8 font_color = Color8::COL8_FFFFFF);
	void set_title(const char* title, Layer_mgr& lymgr, Color8 font_color = Color8::COL8_FFFFFF);


	void load_img(const char* title = "", Color8 client_color = Color8::COL8_E6E6E6);
	void fill_title_box();

	virtual uint class_size  ()  const 
		{ return sizeof(CWindowLayer);}
	virtual CWindowLayer* class_new(char* addr) const {
		return new(addr) CWindowLayer(*this); 
	}
		
};


class CTextLayer: public Layer
{

public:
	CTextLayer(uint offset_x, uint offset_y, uint width, uint height)
						: Layer(offset_x,offset_y, width,height) {}

	void set_text(const char* p_text, Color8 col = Color8::COL8_FFFFFF);
	void set_text(const char* p_text, Layer_mgr& lymgr, Color8 col = Color8::COL8_FFFFFF);

	virtual uint class_size ()  const
		{ return sizeof(CTextLayer);}
	virtual CTextLayer* class_new(char* addr) const {
		return new(addr) CTextLayer(*this); 
	}

};

class CInputLayer: public Layer
{
private:
	uint _cursor_x = 0;
	uint _cursor_y = 0;
	//stringbuf<> _text;
	
public:
	CInputLayer(uint offset_x, uint offset_y, uint width, uint height)
		: Layer(offset_x,offset_y, width,height){
			_cursor_y = _cursor_x = 0;
		}
	void clear() {};
	void add_key(uint keycode,Layer_mgr& lymgr);

	virtual uint class_size () const
		{ return sizeof(CInputLayer);}
	virtual CInputLayer* class_new(char* addr) const {
		return new(addr) CInputLayer(*this); 
	}

};

class Layer_mgr {
public:
	CMEM_MGR& _mmgr;	
	const uint _MAX_WIDTH, _MAX_HEIGHT, _MAX_LAYERS;	

	uint _end = 0;
	uint _bottom_idx = 1;			// 0 is the buffer layer
	Layer** _layers = 0;			// Layer* _layers[]

	void (*_updateEvent)(const Layer_mgr& lymgr, const CRect& rect) = 0;

public:

	Layer_mgr(CMEM_MGR& mmgr, uint max_width, uint max_height, uint max_layers = 100 ):  
		_mmgr(mmgr), _MAX_WIDTH(max_width), _MAX_HEIGHT(max_height), _MAX_LAYERS(max_layers), _end(0)
	{		
		_layers = reinterpret_cast<Layer**>( _mmgr.malloc(sizeof(Layer*) * _MAX_LAYERS ));
		create_layer( Layer(0,0, _MAX_WIDTH,_MAX_HEIGHT) ); 	//create buffer layer
	}

	Layer_mgr(const Layer_mgr& ) = delete;
	Layer_mgr& operator=(const Layer_mgr& ) = delete;

	~Layer_mgr() { _mmgr.close(); }

	Layer* operator[](uint idx) const {
		return (idx < _end)? _layers[idx] : 0;
	}



	Layer* create_layer(const Layer& ly);
	bool remove_layer(Layer* alayer);
	void move_layer_by(uint id, int dx, int dy);

	uint get_layer_idx(const Layer& ly);
	void set_bottom_idx(uint v) { _bottom_idx = v;}
	void set_to_top(uint layer_idx);
	void set_to_top(const Layer& ly);
	void set_to_bottom(uint layer_idx);
	void set_to_bottom(const Layer& ly);


	char* update(uint x0,uint y0, uint w, uint h);
	char* update(const CRect& rt){
		return update(rt._x, rt._y, rt._w, rt._h);
	}
	char* refresh() {
		return update(0,0,_MAX_WIDTH,_MAX_HEIGHT); 
		
	}

	void updateEvent(const CRect& rect) const {
		if(_updateEvent) _updateEvent(*this, rect);
	}
	void print();
	void print_mem(){_mmgr.print();}

};





#endif