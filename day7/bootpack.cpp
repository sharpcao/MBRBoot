#include <stdio.h>
#include "vga.h"
#include "int.h"
#include "input_device.h"

CEventBuf<64> KeyBuf;
CEventBuf<128> MouseBuf;

void int21_handler()
{
    io_out8(PIC::PIC0_OCW2,0x61);
    KeyBuf.set_char(io_in8(PORT::PORT_KEYDAT));

}

void int2c_handler(int* esp)
{
    io_out8(PIC::PIC1_OCW2,0x64);
    io_out8(PIC::PIC0_OCW2,0x62);
    MouseBuf.set_char(io_in8(PORT::PORT_KEYDAT));


}

// __declspec(naked)
// void asm_handler(void(*f)(int*))
// {
//     __asm{
//         push ebp
//         mov  ebp,esp
//         push es
//         push ds
//         pushad
//         mov  eax,esp
//         push eax
//         mov ax,0x08*2
//         mov ds,ax
//         mov es,ax
//         mov eax, [ebp+8] // f
//         //mov eax, f
//         call eax
//         pop eax
//         popad
//         pop ds
//         pop es
//         mov esp,ebp
//         pop ebp
//         add esp , 4
//         iretd
//     }
// }
__declspec(naked)
void asm_IX21()
{
    __asm{
        call int21_handler
        iretd
    }
}

__declspec(naked)
void asm_IX2C()
{
    __asm{
        call int2c_handler
        iretd
    }
}

void message_handle();

void os_main(BOOTINFO *pbi)
{

    CPalette pal(pbi);
    pal.init_palette();
    pal.init_cursor();

    GDTIDT gdtidt;
    gdtidt.init_gdt_idt();
    
    gdtidt.add_idt_handler(0x21, asm_IX21);
    gdtidt.add_idt_handler(0x2c, asm_IX2C);
    //gdtidt.add_idt_handler(0x21, __declspec(naked) []()-> void {asm_handler(int21_handler);}  );
    //gdtidt.add_idt_handler(0x27, []()-> void {asm_handler(int2c_handler);}  );
    //gdtidt.add_idt_handler(0x2c, []()-> void {asm_handler(int2c_handler);}  );

    gdtidt.load_gdt();
    gdtidt.load_idt();
    gdtidt.init_pic();

    io_sti();

    pal.draw_desktop();
    pal.draw_cursor(160,50);

    pal.xyprint(8,8,"ABC 123");
    pal.xyprint(28,28,"Hello World OS!", Color8::COL8_FFFF00);
    CInput_Device dev;
    dev.init_keyboard();
    dev.enable_mouse();

    io_out8(PIC::PIC0_IMR, 0xf9); 
    io_out8(PIC::PIC1_IMR, 0xef); 
    message_handle();

}

void message_handle()
{
    CPalette pal;
    char c, s[10];
    for(;;){
        io_cli();
        
        if ( (c=KeyBuf.get_char()))
        {
            io_sti();
            sprintf((char*)s,"%02X",c);
            pal.boxfill(Color8::COL8_FFFFFF,0,46,80,62);
            pal.xyprint(0,46,(char*)s,Color8::COL8_840084);           
        }else if ( (c=MouseBuf.get_char()) )
        {        
            io_sti();
            sprintf((char*)s,"%02X",c);
            pal.boxfill(Color8::COL8_FFFFFF,0,66,80,82);
            pal.xyprint(0,66,(char*)s,Color8::COL8_840084);  
        }else
        {
            io_stihlt();
        }
    }
}





void (*jump)(BOOTINFO *pbi)  = os_main;
uint _hInstance =0;
