#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "common.h"

uint uint2str(char* p, uint v)
{
	char buf[10];
	int i = 0,size = 0;
	if(v==0){
		*p++ = 0x30;
		size = 1;
	}else{
		for(;v!=0; v/=10,++i){
			buf[i] = (v % 10) + 0x30;
		}
		size = i--;
		for(;i>=0;--i,++p){
			*p = buf[i];
		}
	}
	*p = 0;
	return size;
}

template<uint N=256>
class stringbuf{
public:
	stringbuf() { reset();}

	void reset(){
		_idx = 0;
		for(uint i = 0; i<N; ++i) _buf[i] = 0;
	}

	stringbuf(const stringbuf& rhs) {
		_set_text(rhs.c_str());
	}
	stringbuf(const char* p_text) {
		_set_text(p_text);
	}

	stringbuf& operator=(const stringbuf& rhs) {
		_set_text(rhs.c_str());
		return *this;
	}

	stringbuf& operator=(const char* p_text) {
		_set_text(p_text);	
		return *this;
	}

	stringbuf& operator=(uint v){
		_idx = uint2str(&_buf[_idx],v)+1;
		return *this;

	}

	stringbuf&  operator<<(uint v)
	{
		_idx += uint2str(&_buf[_idx],v);
		return *this;
	}
	stringbuf& operator<<(const char* p)
	{
		for(;*p!=0;p++){
			_buf[_idx++] = *p;
		}
		_buf[_idx] = 0;
		return *this;
	}
	stringbuf& operator<<(char c){
		_buf[_idx++] = c;
		_buf[_idx] = 0;
		return *this;
	}

	uint size() {return (_idx>0)? _idx - 1: 0;}
	const char* c_str() const {return _buf;}

private:
	uint _idx;
	const uint _max = N;
	char _buf[N];
	void _set_text(const char* p_text){
		for(_idx = 0; _idx <N-1; ++_idx){
			_buf[_idx] = p_text[_idx];
			if(p_text[_idx] == 0) break;
		}
		_buf[N-1] = 0;
	}
};



template<typename T>
class Array{
	static constexpr uint _end = 64;
	T _data[_end];
	uint _last = 0;
public:
	bool push_back(const T& v);
	uint size() { return _last;}
	uint capacity(){return _end;}
	bool erase(const T& v);
	T operator[] (uint i) { return _data[i]; }
	bool has(const T& v);

};

template<typename T>
bool Array<T>::push_back(const T& v)
{
	if(_last == _end) {
		return false;
	}else{
		_data[_last++] = v;
	}
	return true;
}

template<typename T>
bool Array<T>::erase(const T& v)
{
	bool result = false;
	for(uint i = 0; i < _last; ++i)
	{
		if(_data[i] == v){
			for(uint j = i+1; j < _last; ++j)
			{
				_data[j-1] = _data[j];
			}
			--_last;
			result = true;
		}
	}
	return result;
}

template<typename T>
bool Array<T>::has(const T& v)
{
	bool result = false;
	for(uint i = 0; i < size(); ++i){
		if(_data[i] == v) {
			result = true;
			break;
		}
	}
	return result;
}

#endif
