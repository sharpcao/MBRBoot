#ifndef HANDLER_H
#define HANDLER_H

#include "common.h"
#include "layer.h"

using HF = void (*)();
void int21_handler();
void int2c_handler();

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


void handle_message();



#endif