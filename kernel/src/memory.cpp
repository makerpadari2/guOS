#include "memory.h"

/*
Functions for memory handling
*/

// Paramaters: Pointer to memory map, ammount of memory map entries,
// size of the memory map
uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, 
uint64_t mMapDescSize) 
{   
    // Used for holding memory size, globally declared
    static uint64_t memorySizeBytes = 0;    

    // If memory size was already calculated
    if (memorySizeBytes > 0) {
        return memorySizeBytes;
    }

    /* Iterate through every map entry and add up the size of that 
    memory to memorySizeBytes */
    for (int i = 0; i< mMapEntries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap +
        (i * mMapDescSize));

        memorySizeBytes += desc->numPages * 4096;
    }
    return memorySizeBytes;
}

// Memory set function used in Page Table Manager
void memset(void* start, uint8_t value, uint64_t num) {
    for (uint64_t i = 0; i < num; i++) {

        // Cast value to uint64 and then to uint8
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}