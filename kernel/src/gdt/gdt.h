#pragma once
#include <stdint.h>

/*
Header file used in gdt.cpp and kernelUtil.h
*/

// GDTDescriptor lets the CPU know where the GDT is and how large it is
struct GDTDescriptor {
    uint16_t Size;
    uint64_t Offset;
} __attribute__((packed));

struct GDTEntry {

    // Lowest 16 bits of limit and base
    uint16_t Limit0;
    uint16_t Base0;

    uint8_t Base1;

    // Flags each memory segment has
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
} __attribute__((packed));

struct GDT {
    GDTEntry Null;          // 0x00
    GDTEntry KernelCode;    // 0x08
    GDTEntry KernelData;    // 0x10
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;
} __attribute__((packed)) __attribute((aligned(0x1000)));

// Declare GDT
extern GDT DefaultGDT;

// LoadGDT (gdt.asm)
extern "C" void LoadGDT(GDTDescriptor* gdtDescriptor);