#pragma once
#include "../BasicRenderer.h"
#include "../userinput/mouse.h"

/*
Header file used in interrupts.cpp and kernelUtil.h
*/

#define PIC1_COMMAND 0X20   // Command line of PIC chip
#define PIC1_DATA 0X21      // Data line of PIC chip
#define PIC2_COMMAND 0XA0   // Command line of slave PIC chip
#define PIC2_DATA 0XA1
#define PIC_EOI 0X20

#define ICW1_INIT 0X10
#define ICW1_ICW4 0X01
#define ICW4_8086 0X01

struct interrupt_frame;
__attribute__((interrupt)) void PageFault_handler(interrupt_frame* frame);
__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame);
__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame);
__attribute__((interrupt)) void KeyboardINT_handler(interrupt_frame* frame);
__attribute__((interrupt)) void MouseINT_handler(interrupt_frame* frame);

void RemapPIC();

// Functions for ending interrupt on PIC chip
void PIC_EndMaster();
void PIC_EndSlave();