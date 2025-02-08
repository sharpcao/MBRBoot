#ifndef CEVENTBUF_H
#define CEVENTBUF_H

template<uint N>
class CEventBuf
{

public:
	
	bool set_char(char c);
	char get_char();
	bool is_empty() { return (_i1 == _i2);}

private:
	uint _i1=0, _i2 = 0;
	char _data[N] ={0};
	
};

template<uint N>
bool CEventBuf<N>::set_char(char c)
{
	if ( (_i2 + 1 ) % N == _i1 ){
		return false;
	}else{
		_data[_i2++] = c;
		if (_i2 == N) {_i2 = 0;}
		return true;
	}
}

template<uint N>
char CEventBuf<N>::get_char()
{
	char c = 0;
	if (_i1 != _i2){
		c = _data[_i1++];
		if(_i1 == N) {_i1 = 0;}
		if(_i1 == _i2) {_i1 = _i2 = 0;}
	}
	return c;
}


#endif