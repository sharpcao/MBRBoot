#ifndef OS_IO_H
#define OS_IO_H

using uint = unsigned int;
inline void io_out8(uint port, uint data)
{
	asm{
		mov edx, port
		mov eax, data
		out dx, al 
	}
}

inline uint io_load_eflags()
{
	uint eflags;
	asm{
		pushfd 
		pop eflags 
	}
	return eflags;
}
inline void io_cli()
{
	asm {
		cli
	}
}

inline void io_store_eflags(uint eflags)
{
	asm{
		push eflags 
		popfd
	}
}



#endif

