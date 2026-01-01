#ifndef CONSOLE_API_H
#define CONSOLE_API_H

#include "common.h"


__declspec(naked) extern "C"
void __stdcall api_jmp_func();

__declspec(naked) extern "C"
void __stdcall api_print_char1(uchar c);


__declspec(naked) extern "C"
void __stdcall api_print_char2(uchar c);


typedef uint  API_ADDR;


extern API_ADDR API_Entry[10];


#endif