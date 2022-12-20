#pragma once
#include <stdint.h>
#include "efiMemory.h"

/*
Header file for PageFrameAllocator.h and memory.cpp
*/

/* Get total memory of system */
uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, 
uint64_t mMapDescSize);

void memset(void* start, uint8_t value, uint64_t num);