#include <stdio.h>
#include "vga.h"
#include "int.h"

void int21_handler(int* esp)
{

    CPalette pal;
   
    pal.draw_desktop();
    pal.boxfill(Color8::COL8_FFFFFF,0,0,32*8-1,15);
    pal.xyprint(0,0,"INT 21 (IRQ-1):PS/2 keyboard",Color8::COL8_840084);
    for(;;){
        __asm { hlt}
    }
}
void int2c_handler(int* esp)
{

    CPalette pal;

    pal.draw_desktop();
    pal.boxfill(Color8::COL8_FFFFFF,0,0,32*8-1,15);
    pal.xyprint(0,0,"INT 2C (IRQ-12) : PS/2 mouse",Color8::COL8_840084);
    for(;;){
        __asm { hlt}
    }
}


void asm_handler(void(*f)(int*))
{
    __asm{
        push es
        push ds
        pushad
        mov  eax,esp
        push eax
        mov ax,0x08*2
        mov ds,ax
        mov es,ax
        //mov eax, int21_handler
        mov eax, f
        call eax
        pop eax
        popad
        pop ds
        pop es
        iretd
    }
}

void os_main(BOOTINFO *pbi)
{

    CPalette pal(pbi);
    pal.init_palette();
    pal.init_cursor();

    GDTIDT gdtidt;
    gdtidt.init_gdt_idt();
    gdtidt.add_idt_handler(0x21, []()-> void {asm_handler(int21_handler);}  );
    gdtidt.add_idt_handler(0x27, []()-> void {asm_handler(int2c_handler);}  );
    gdtidt.add_idt_handler(0x2c, []()-> void {asm_handler(int2c_handler);}  );

    gdtidt.load_gdt();
    gdtidt.load_idt();
    gdtidt.init_pic();

    io_sti();

    pal.draw_desktop();
    pal.draw_cursor(160,50);

    pal.xyprint(8,8,"ABC 123");
    pal.xyprint(31,31,"Hello World OS!", Color8::COL8_FFFF00);

    io_out8(PIC::PIC0_IMR, 0xf9); 
    io_out8(PIC::PIC1_IMR, 0xef); 
    for(;;){
        __asm{ hlt}
    }

}

void (*jump)(BOOTINFO *pbi)  = os_main;
