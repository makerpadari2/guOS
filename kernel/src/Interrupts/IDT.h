#pragma once
#include <stdint.h>

/*
Header file used in IDT.cpp and kernelUtil.cpp
*/

// Define interrupt types
#define IDT_TA_InterruptGate 0b10001110
#define IDT_TA_CallGate     0b10001100
#define IDT_TA_TrapGate     0b10001111

// IDTDescEntry handles info about what to do when a certain interrupt is cought
struct IDTDescEntry {
    uint16_t offset0;   // Lower 16 bits of offset
    uint16_t selector;  // Segment selector used when interrupt is cought
    uint8_t ist;        // Holds interrupt stack table offset
    uint8_t type_attr;  // What type of interrupt is cought
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;
    void SetOffset(uint64_t offset);
    uint64_t GetOffset();
};

// IDTR contains info stored in the IDT register (like size and offset of IDT)
struct IDTR {
    uint16_t Limit;
    uint64_t Offset;
} __attribute__((packed));