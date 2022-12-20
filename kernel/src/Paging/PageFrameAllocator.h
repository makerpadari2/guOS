#pragma once
#include "../efiMemory.h"
#include <stdint.h>
#include "../Bitmap.h"
#include "../memory.h"

/*
Header file used in PageFrameAllocator.cpp and PageTableManager.cpp
*/

class PageFrameAllocator {
    public:

    // Function used to let the page frame allocator know what sections of 
    // memory are being handled
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, 
    size_t mMapDescSize);

    // Bitmap used for page frame allocator
    Bitmap PageBitmap;

    // Function prototypes for lock and free pages
    void FreePage(void* address);
    void FreePages(void* address, uint64_t pageCount);
    void LockPage(void* address);
    void LockPages(void* address, uint64_t pageCount);

    // For requesting a free page
    void* RequestPage();

    uint64_t GetFreeRAM();
    uint64_t GetUsedRAM();
    uint64_t GetReservedRAM();

    // Function prototypes for initializing bitmap and reserving and unreserving
    // pages
    private:
    void InitBitmap(size_t bitmapSize, void* bufferAddress);
    void ReservePage(void* address);
    void ReservePages(void* address, uint64_t pageCount);
    void UnreservePage(void* address);
    void UnreservePages(void* address, uint64_t pageCount);
};

// Global instance of PageFrameAllocator that is usable outside of kernel
extern PageFrameAllocator GlobalAllocator;