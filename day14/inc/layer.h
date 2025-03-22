#ifndef LAYER_H
#define LAYER_H

#include "common.h"
#include "vga.h"
#include "mem_mgr.h"
#include "functions.h"

class CLayer_Mgr;

class CLayer{
protected:
	uint _offset_x = 0, _offset_y = 0;
	uint _width = 0, _height = 0;
	bool _visible = true;
	char* _img_data = 0;  // _width * _height 
public:
	CLayer(uint offset_x, uint offset_y, uint width, uint height)
			:_offset_x(offset_x), _offset_y(offset_y),_width(width),_height(height)
			{}
	CLayer(const CLayer&) = default;

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
	CRect get_area() const { return CRect(_offset_x,_offset_y,_width,_height);}

	virtual uint class_size() const  { return sizeof(CLayer);}
	virtual CLayer* class_new_at(char* addr) const ;
	
	friend class CLayer_Mgr;


};


class CDesktopLayer: public CLayer
{
public:
	CDesktopLayer(uint scrnx, uint scrny): CLayer(0,0,scrnx,scrny) {}
	void load_img();
	virtual uint class_size ()  const
		{ return sizeof(CDesktopLayer);}
	virtual CDesktopLayer* class_new_at(char* addr) const {
		return new(addr) CDesktopLayer(*this); 
	}
		
};

class CWindowLayer: public CLayer
{
private:
	//static char close_btn = [14][16];
	stringbuf<> _title;
public:
	CWindowLayer(uint offset_x, uint offset_y, uint width, uint height)
						: CLayer(offset_x,offset_y, width,height) {}
	void load_img(const char* title = "");
	virtual uint class_size  ()  const 
		{ return sizeof(CWindowLayer);}
	virtual CWindowLayer* class_new_at(char* addr) const {
		return new(addr) CWindowLayer(*this); 
	}
		
};

class CTextLayer: public CLayer
{

public:
	CTextLayer(uint offset_x, uint offset_y, uint width, uint height)
						: CLayer(offset_x,offset_y, width,height) {}

	void set_text(const char* p_text, Color8 col = Color8::COL8_FFFFFF);
	void set_text(const char* p_text, CLayer_Mgr& lymgr, Color8 col = Color8::COL8_FFFFFF);

	virtual uint class_size ()  const
		{ return sizeof(CTextLayer);}
	virtual CTextLayer* class_new_at(char* addr) const {
		return new(addr) CTextLayer(*this); 
	}

};

class CInputLayer: public CLayer
{
private:
	uint _cursor_x = 0;
	uint _cursor_y = 0;
	//stringbuf<> _text;
	
public:
	CInputLayer(uint offset_x, uint offset_y, uint width, uint height)
		: CLayer(offset_x,offset_y, width,height){
			_cursor_y = _cursor_x = 0;
		}
	void clear() {};
	void add_key(uint keycode,CLayer_Mgr& lymgr);

	virtual uint class_size () const
		{ return sizeof(CInputLayer);}
	virtual CInputLayer* class_new_at(char* addr) const {
		return new(addr) CInputLayer(*this); 
	}

};

class CLayer_Mgr {
public:
	CMEM_MGR& _mmgr;	
	const uint _max_width, _max_height;	
	const uint _max_layers;
	uint _last = 0;
	uint _bottom_idx = 1;			// 0 is the buffer layer
	CLayer** _layers =0;			// CLayer* _layers[]
	void (*_updateEvent)(const CLayer_Mgr& lymgr, const CRect& rect) =0;

public:

	CLayer_Mgr(CMEM_MGR& mmgr, uint max_width, uint max_height, uint max_layers=100):  
		_mmgr(mmgr), _max_width(max_width), _max_height(max_height),_max_layers(max_layers),_last(0)
	{		
		_layers = (CLayer**)_mmgr.malloc(sizeof(CLayer*)*_max_layers);

		add_layer(CLayer(0,0, _max_width,_max_height)); //add buf layer
	}
	CLayer* operator[](uint idx) const {
		return (idx < _last)? _layers[idx] : 0;
	}


	CLayer_Mgr(const CLayer_Mgr&) = delete;
	CLayer_Mgr& operator=(const CLayer_Mgr&) = delete;

	~CLayer_Mgr() { _mmgr.close(); }

	CLayer* add_layer(const CLayer& ly);
	bool remove_layer(CLayer* alayer);
	void move_layer_by(uint id, int dx, int dy);

	uint get_layer_idx(const CLayer& ly);
	void set_bottom_idx(uint v) { _bottom_idx = v;}
	void set_to_top(uint layer_idx);
	void set_to_top(const CLayer& ly);
	void set_to_bottom(uint layer_idx);
	void set_to_bottom(const CLayer& ly);


	char* update(uint x0,uint y0, uint w, uint h);
	char* update(const CRect& rt){
		return update(rt._x, rt._y, rt._w, rt._h);
	}
	char* refresh() {
		return update(0,0,_max_width,_max_height); 
		
	}

	void updateEvent(const CRect& rect) const {
		if(_updateEvent) _updateEvent(*this, rect);
	}
	void print();
	void print_mem(){_mmgr.print();}

};





#endif