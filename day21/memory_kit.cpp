#include "inc\memory_kit.h"


template<typename T>
void swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

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

uint CMEM_MGR::malloc(uint size)
{
	uint addr = 0;
	if(_last>0 && (addr=_mf[0].malloc(size)) > 0) sort_down(0);
	return addr;
}


void CMEM_MGR::sort_down(uint nitem)
{
	for(uint i = nitem; i<_last -1; ++i){
		if (_mf[i].size() < _mf[i+1].size()){
			swap(_mf[i],_mf[i+1]);
		}else{
			break;
		}
	}
	reduce();
}

void CMEM_MGR::sort_up(uint nitem)
{
	for(uint i = nitem; i > 1; --i){
		if (_mf[i].size() > _mf[i-1].size()){
			swap(_mf[i],_mf[i-1]);
		}else{
			break;
		}
	}
}

void CMEM_MGR::free(uint addr, uint size)
{
	int left = -1, right = -1;
	MEM_FREE_REC rec(addr,size);
	for(uint i = 0; i< _last; ++i){
		if(_mf[i].end() == rec.start()) left = i;
		if(_mf[i].start() == rec.end()) right = i;
		if(left>=0 & right >= 0) break;
	}
	DEBUG_BUILD(std::cout << "## mgr:"<<this <<" free:" << std::hex << rec.start() <<":" <<rec.size() <<","<<std::dec 
					<<left <<":" <<right <<std::endl;)
	free_with(rec, left,right);
}

void CMEM_MGR::free_with(MEM_FREE_REC& rec, int left, int right)
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
		if(_last < _max){
			_mf[_last++] = rec;
		}
	}
}

void CMEM_MGR::reduce()
{
	for(; _last != 0; --_last)
		if (_mf[_last-1].size()>0) break;
}

void CMEM_MGR::print(stringbuf<>& scout) const
{
	stringbuf<> this_addr;
	this_addr << stringbuf<>::flag::hex << (uint)this;
	if (_last==0){
		scout << "mgr:" << this_addr << " is empty\n";
	}else {
		stringbuf<> sstart, send;
		for(uint i = 0; i<_last; ++i){
			sstart.reset();
			send.reset();
			sstart << stringbuf<>::flag::hex << _mf[i].start();
			send << stringbuf<>::flag::hex <<_mf[i].end();
			scout << "mgr:" << this_addr 
				 << " free_entry:"<< i << ":" <<sstart << "~" << send<< "\n";
		}
	}

}

uint CMEM_MGR::get_mem_free() const
{
	uint result = 0;
	if ( _last){
		for(uint i = 0; i<_last; ++i)	result += _mf[i].size();
	}
	return result;
}