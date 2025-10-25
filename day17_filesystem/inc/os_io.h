#ifndef OS_IO_H
#define OS_IO_H
using uint=unsigned int;
using uchar=unsigned char;
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
void io_out8(uint port, uint value);

__declspec(naked) uchar io_in8(uint port);
__declspec(naked) void io_cli();
__declspec(naked) void io_sti();
__declspec(naked) void io_hlt();
__declspec(naked) void io_stihlt();


//__declspec(naked)  
uint io_load_eflags();

//__declspec(naked) 
void  io_store_eflags(uint eflags);

__declspec(naked) void io_load_tr(uint idx);
__declspec(naked) void task_switch(uint ip, uint cs);
__declspec(naked) void jmp_cs(short int);

__declspec(naked) uint load_cr0();
__declspec(naked) void store_cr0(uint v);


struct OS_LOAD_GDT;
struct OS_LOAD_IDT;

void load_gdt(OS_LOAD_GDT* ptr);
void load_idt(OS_LOAD_IDT* ptr);

namespace cpu {
	struct Reg{
			uint eax, ebx, ecx, edx, esi, edi;
			uint cs, ds, es,fs, gs;
			uint eflags;
	};
	__declspec(naked) void get_regs(Reg* reg);
}

#endif

