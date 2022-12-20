#pragma once
#include <stdint.h>

/*
Header file used in efiMemory.cpp, PageFrameAllocator.h and kernelUtil.h
*/

struct EFI_MEMORY_DESCRIPTOR {
    uint32_t type;      // Type of memory section
    void* physAddr;     // Pointer to the address the memory section is at
    void* virtAddr;     // Pointer to the virtual address the memory is mapped to
    uint64_t numPages;  // Number of pages
    uint64_t attribs;   // Attributes/extra information
};

// For strings with different types of memory descriptors
extern const char* EFI_MEMORY_TYPE_STRINGS[];