#ifndef BOOTPACH_H
#define BOOTPACH_H

#include "vga.h"
#include "int.h"
#include "input_device.h"
#include "handler.h"
#include "functions.h"
#include "memtest.h"
#include "layer.h"
#include "global.h"

void init_layers(CMEM_MGR& mem_mgr, uint width, uint height);
void timer0_timeout();
void timer1_timeout();
#endif