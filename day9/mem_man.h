#ifndef MEM_MAN_H
#define MEM_MAN_H

#include <iostream>
using uint = unsigned int;

class MEM_FREE_REC{
public:
	MEM_FREE_REC():_addr(0), _size(0) {}
	MEM_FREE_REC(uint addr, uint size) : _addr(addr), _size(size) {}

	uint size() const { return _size;}
	uint start() const { return _addr;}
	uint end() const {return _addr + _size;}
	void zero() {_addr = _size = 0;}

	uint malloc(uint size);
	MEM_FREE_REC& operator+(const MEM_FREE_REC& rhs)

private:
	uint _addr=0, _size=0;
};

template<uint N>
class CMEM_MAN {
public:
	CMEM_MAN(){};
	uint malloc(uint size);
	void free(uint addr, uint size);
	void print();
private:
	void free_with(MEM_FREE_REC& rec, int left, int right);
	void sort_down(uint i);
	void sort_up(uint i);
	void reduce();

	MEM_FREE_REC _mf[N];
	uint _n = 0;
	const uint _max = N;
};


uint MEM_FREE_REC::malloc(uint size)
{
	if(size <= _size){
		_addr += size;
		_size -= size;
		return _addr - size;
	}else{
		return 0;
	}
}

MEM_FREE_REC& MEM_FREE_REC::operator+(const MEM_FREE_REC& rhs)
{
	if (end() == rhs.start()){
		_size += rhs.size();
	}else if(start()==rhs.end()){
		_addr = rhs.start();
		_size += rhs.size();
	}
	return *this;
}


template<typename T>
void swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

template<uint N>
uint CMEM_MAN<N>::malloc(uint size)
{
	uint addr = 0;
	if(_n>0 && (addr=_mf[0].malloc(size)) > 0) sort_down(0);
	return addr;
}

template<uint N>
void CMEM_MAN<N>::sort_down(uint nitem)
{
	for(uint i = nitem; i<_n -1; ++i){
		if (_mf[i].size() < _mf[i+1].size()){
			swap(_mf[i],_mf[i+1]);
		}else{
			break;
		}
	}
	reduce();
}

template<uint N>
void CMEM_MAN<N>::sort_up(uint nitem)
{
	for(uint i = nitem; i > 1; --i){
		if (_mf[i].size() > _mf[i-1].size()){
			swap(_mf[i],_mf[i-1]);
		}else{
			break;
		}
	}
}

template<uint N>
void CMEM_MAN<N>::reduce()
{
	for(; _n != 0; --_n)
		if (_mf[_n-1].size()>0) break;
}


template<uint N>
void CMEM_MAN<N>::free(uint addr, uint size)
{
	int left = -1, right = -1;
	MEM_FREE_REC rec(addr,size);
	for(uint i = 0; i< _n; ++i){
		if(_mf[i].end() == rec.start()) left = i;
		if(_mf[i].start() == rec.end()) right = i;
		if(left>=0 & right >= 0) break;
	}
	std::cout << "free:" << rec.start() <<":" <<rec.size() <<","<<left <<":" <<right <<std::endl;
	free_with(rec, left,right);
}

template<uint N>
void CMEM_MAN<N>::free_with(MEM_FREE_REC& rec, int left, int right)
{
	if (left >=0 && right >=0){
		_mf[right] =  _mf[left] + rec+_mf[right] ;
		_mf[left].zero();
		sort_up(right);
		sort_down(left);
	}else if(left >= 0){
		_mf[left] = _mf[left] +  rec;
		sort_up(left);
	}else if(right >= 0){
		_mf[right] = rec + _mf[right];
		sort_up(right);
	}else{
		if(_n < N){
			_mf[_n++] = rec;
		}
	}
}

template<uint N>
void CMEM_MAN<N>::print()
{
	for(uint i = 0; i<_n; ++i){
		std::cout <<"mem:"<< i << ":" << _mf[i].start() << "~" << _mf[i].end() << std::endl;
	}
}

#endif