#include "IDT.h"
/*
Interrupt Descriptors Table is used to configure interrupts
*/

void IDTDescEntry::SetOffset(uint64_t offset) {

    // Put lower 16 bits of offset into offset0
    offset0 = (uint16_t)(offset & 0x000000000000ffff);       

    offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

uint64_t IDTDescEntry::GetOffset(){
    uint64_t offset = 0;
    offset |= (uint64_t)offset0;
    offset |= (uint64_t)offset1 << 16;
    offset |= (uint64_t)offset2 << 32;
    return offset;
}