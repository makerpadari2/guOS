#include "paging.h"

/*
Used to define page directory entry (in paging.h)
*/

void PageDirectoryEntry::SetFlag(PT_Flag flag, bool enabled){

    // Bitshift to the left by 1 (ReadWrite)
    uint64_t bitSelector = (uint64_t)1 << flag;

    // Set bit to 0
    Value &= ~bitSelector;

    // If enabled set bit back to 1
    if (enabled){
        Value |= bitSelector;
    }
}

bool PageDirectoryEntry::GetFlag(PT_Flag flag){
    uint64_t bitSelector = (uint64_t)1 << flag;

    // If bit is enabled then true otherwise false
    return Value & bitSelector > 0 ? true : false;
}

uint64_t PageDirectoryEntry::GetAddress(){
    return (Value & 0x000ffffffffff000) >> 12;
}

void PageDirectoryEntry::SetAddress(uint64_t address){
    // Bitmask address so it only has 40 bits
    address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (address << 12);
}