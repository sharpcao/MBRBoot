#ifndef FUNCTIONS_H
#define FUNCTIONS_H
inline int set_inrange(int v, int low, int high)
{
    return (v < low)? low :((v>high)? high :v);
}

inline int min(int x,int y)
{
	return (x<y)?x:y;
}



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

template<uint N>
class stringbuf{
public:
	stringbuf() { reset();}
	void reset(){
		_idx = 0;
		for(uint i = 0; i<N; ++i) _buf[i] = 0;
	}

	stringbuf(const stringbuf&) = delete;
	stringbuf& operator=(const stringbuf&) = delete;

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
	const char* get_string() {return _buf;}

private:
	uint _idx;
	char _buf[N];
};

#endif
