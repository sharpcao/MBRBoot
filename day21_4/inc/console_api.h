#ifndef CONSOLE_API_H
#define CONSOLE_API_H

#include "common.h"


__declspec(naked) extern "C"
void __stdcall api_jmp_func();


__declspec(naked) extern "C"
void __stdcall api_print_char(uchar c);


typedef uint  API_ADDR;


//extern  API_ADDR API_Entry[10];


extern uint int03_print_message();

#endif