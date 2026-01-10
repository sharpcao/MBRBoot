#ifndef HANDLER_H
#define HANDLER_H

#include "common.h"
#include "layer.h"
#include "console_api.h"

using HF = void (*)();
using HF_SAFE = void(*)(uint int_num);
void int20_handler();
void int21_handler();
void int2c_handler();
void int7_handler(); //clear TS
void int0a_handler(uint int_num);

__declspec (naked) void int40_handler();
__declspec (naked) void int0d_handler();


template<HF handler_func> 
__declspec(naked)
void handler_wrap()
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
    }

    handler_func();

    __asm{
        pop eax
        popad
        pop ds
        pop es
        iretd
    }
}

template<HF_SAFE handler_func, constexpr uint int_num> 
__declspec(naked)
void handler_wrap_safe()
{
    __asm{
        mov ax,0x08*2
        mov ds,ax
        mov es,ax
        mov ax, 0x08*3
        mov ss,ax
        mov esp, 0x7c00
    }

    handler_func(int_num);

    __asm{
        iretd
    }
}



void handle_message();



#endif