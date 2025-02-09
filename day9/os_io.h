#ifndef OS_IO_H
#define OS_IO_H
using uint=unsigned int;
enum PIC {
		PIC0_ICW1=0x0020,
		PIC0_OCW2=0x0020,
		PIC0_IMR=0x0021,
		PIC0_ICW2=0x0021,
		PIC0_ICW3=0x0021,
		PIC0_ICW4=0x0021,
		PIC1_ICW1=0x00a0,
		PIC1_OCW2=0x00a0,
		PIC1_IMR=0x00a1,
		PIC1_ICW2=0x00a1,
		PIC1_ICW3=0x00a1,
		PIC1_ICW4=0x00a1
	};

//__declspec(naked) 
void io_out8(uint port, uint value)
{
	__asm{
		mov edx, port
		mov eax, value
		out dx,al
		//ret
	}
}
__declspec(naked)
char io_in8(uint port)
{
	__asm{
		mov edx, [esp+4]
		mov eax,0
		in al,dx
		ret
	}
}

__declspec(naked)  
uint io_load_eflags()
{
	__asm{
		pushfd 
		pop eax
		ret
	}
}

__declspec(naked) 
void io_cli()
{
	__asm {
		cli
		ret
	}
}

__declspec(naked) 
void io_sti()
{
	__asm{
		sti
		ret
	}
}
__declspec(naked) 
void io_stihlt()
{
	__asm{
		sti
		hlt
		ret
	}
}



__declspec(naked) 
void io_store_eflags(uint eflags)
{
	__asm{
		push dword ptr [esp+4] 
		popfd
		ret
	}
}





struct SEGMENT_DESCRIPTOR
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
	void set(uint base, uint limit, uint ar);
	
};


struct GATE_DESCRIPTOR
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
	void set(uint offset, uint selector, uint ar);
};

constexpr uint AR_INTGATE32 =0x008e;

void SEGMENT_DESCRIPTOR::set(uint limit, uint base, uint ar)
{
	// limit =0xffffffff, base=0x00000000 ar = 0x409a
	if (limit > 0xfffff){
		ar |= 0x8000;	// G_bit = 1
		limit /= 0x1000;
	}
	limit_low = limit & 0xffff;
	base_low = base & 0xff;
	base_mid = (base >> 16) & 0xff;
	access_right = ar & 0xff;
	limit_high = ((limit >> 16) & 0x0f) | ((ar>>8) & 0xf0);
	base_high = (base >> 24) & 0x0f;
}

void GATE_DESCRIPTOR::set(uint offset, uint sel, uint ar)
{
	offset_low = offset & 0xffff;
	selector = sel;
	dw_count = (ar>>8) & 0xff;
	access_right = ar & 0xff;
	offset_high = (offset >> 16) & 0xffff;

}

#pragma pack(push, 1)
struct  OS_LOAD_GDT
{
	short limit;
	SEGMENT_DESCRIPTOR*  base;
	OS_LOAD_GDT() = delete;
	OS_LOAD_GDT(short limit, SEGMENT_DESCRIPTOR* base) : limit(limit), base(base)
	{
		auto* p = this;
		__asm {
			mov	 eax, p
			lgdt [eax]
		};
	}
};

struct OS_LOAD_IDT
{
	short limit;
	GATE_DESCRIPTOR*  base;
	OS_LOAD_IDT() = delete;
	OS_LOAD_IDT(short limit, GATE_DESCRIPTOR* base) : limit(limit), base(base)
	{
		auto* p = this;
		__asm {
			mov	 eax, p
			lidt [eax]
		}
	}
};



#pragma pack(pop) 

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

