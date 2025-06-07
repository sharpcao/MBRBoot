#ifndef MESSAGE_KIT_H
#define MESSAGE_KIT_H

template<uint N = 128>
class Message_mgr
{

public:
	
	bool push_message(uint p1, uint p2);
	bool get_message(uint& p1, uint& p2);
	bool is_empty() { return (_i1 == _i2);}

private:
	uint _i1 = 0, _i2 = 0;
	uint _param1[N];
	uint _param2[N];
	
};


template<uint N>
bool Message_mgr<N>::push_message(uint p1, uint p2)
{
	if( (_i2+1) % N == _i1 ){
		return false;
	}else{
		_param1[_i2] = p1;
		_param2[_i2] = p2;
		++_i2;
		if (_i2 == N) _i2 = 0;
		return true;
	}
}

template<uint N>
bool Message_mgr<N>::get_message(uint& p1, uint& p2)
{
	//p1 = p2 = 0;
	if(_i1 == _i2) return false;
	
	p1 = _param1[_i1];
	p2 = _param2[_i1];
	++_i1;
	if(_i1 == N) _i1 = 0;
	if(_i1 == _i2) {_i1 = _i2 = 0;}
	return true;

}


#endif