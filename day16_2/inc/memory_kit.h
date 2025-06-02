#ifndef MEMORY_KIT_H
#define MEMORY_KIT_H

#include "common.h"

class MEM_FREE_REC{
public:
	MEM_FREE_REC(uint addr=0, uint size=0) : _addr(addr), _size(size) {}
	MEM_FREE_REC(const MEM_FREE_REC&) = default; 

	uint size() const { return _size;}
	uint start() const { return _addr;}
	uint end() const {return _addr + _size;}
	void zero() {_addr = _size = 0;}

	uint malloc(uint size);
	MEM_FREE_REC& operator+(const MEM_FREE_REC& rhs);
	MEM_FREE_REC& operator=(const MEM_FREE_REC& rhs) = default;

private:
	uint _addr, _size;
};


class CMEM_MGR {
public:
	
	CMEM_MGR(uint addr, uint size, CMEM_MGR* parent=0 ) :
			_last(0),_total_addr(addr), _total_size(size), _parent(parent)
		{ reset();}

	void close() { if (_parent) _parent->free(_total_addr,_total_size); }

	uint malloc(uint size);
	void free(uint addr, uint size);
	void reset() {_last= 0; free(_total_addr,_total_size);}
	
	void print();
	uint size() {return _last;}

private:
	void free_with(MEM_FREE_REC& rec, int left, int right);
	void sort_down(uint i);
	void sort_up(uint i);
	void reduce();

	static constexpr  uint _max = 100;
	MEM_FREE_REC _mf[_max];
	uint _last = 0;
	uint _total_addr = 0;
	uint _total_size = 0;
	CMEM_MGR* _parent= 0;
};


#endif