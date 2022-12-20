#include "PageFrameAllocator.h"

/*
The Page frame allocator maintains in use and free pages so 
proccesses wont have conflicting memory
*/

// Free memory of entire system
uint64_t freeMemory;        
uint64_t reservedMemory;  

// Memory requested from proccesses
uint64_t usedMemory;        

// Used so page frame allocator isn't intialized more than once
bool Initialized = false;   
PageFrameAllocator GlobalAllocator;

uint64_t pageBitmapIndex = 0;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, 
size_t mMapSize, size_t mMapDescSize) 
{
    if (Initialized) return;

    Initialized = true;

    uint64_t mMapEntries = mMapSize / mMapDescSize;

    void* largestFreeMemSeg = NULL;
    size_t largestFreeMemSegSize = 0;


    // Loop through memory map entries
    for (int i = 0; i < mMapEntries; i++) {

        // Get pointer to descriptor
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));

        // type = EfiConventionalMemory
        if (desc->type == 7) {  

            // Find largest memory section 
            if (desc->numPages * 4096 > largestFreeMemSegSize) 
            {
                // New largest free memory segment for bitmap
                largestFreeMemSeg = desc->physAddr;            
                largestFreeMemSegSize = desc->numPages * 4096;
            }
        }
    }

    // Get total memory size of the system for bitmap size
    uint64_t memorySize = GetMemorySize(mMap, mMapEntries, mMapDescSize);
    freeMemory = memorySize;
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;

    // Initialize bitmap
    InitBitmap(bitmapSize, largestFreeMemSeg);

    // Lock the pages where bitmap is stored so it 
    // can't be accessed when accessing a page
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);

    // Reserve pages of unusable/reserved memory
    for (int i = 0; i < mMapEntries; i++) {

    // Get pointer to descriptor
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + 
    (i * mMapDescSize));

    if (desc->type != 7) {  // not efiConventionalMemory

        // Go through memory descriptors and reserve non conventionalmemory pages
        ReservePages(desc->physAddr, desc->numPages);   
    }
    }

}

    void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddress) 
    {
        PageBitmap.Size = bitmapSize;                  
        PageBitmap.Buffer = (uint8_t*)bufferAddress;

        // Set memory in bitmap to 0 for correct system memory representation
        for (int i = 0; i < bitmapSize; i++) { 
            *(uint8_t*)(PageBitmap.Buffer + i) = 0;
        }
    }

void PageFrameAllocator::FreePage(void* address){
    uint64_t index = (uint64_t)address / 4096;

    // Check if a pageframe is in use
    if (PageBitmap[index] == false) return;  

    // If it isn't then increase freememory, decrease usedmemory   
    if (PageBitmap.Set(index, false)) {         
        freeMemory += 4096;
        usedMemory -= 4096;
        if (pageBitmapIndex > index) pageBitmapIndex = index;
    }
}



/* Give free page to use in page table to extend memory of proccesses */
void* PageFrameAllocator::RequestPage() {

    // Go through all indexes in bitmap and find free page frame
    for (; pageBitmapIndex < PageBitmap.Size * 8; pageBitmapIndex++) {
        if (PageBitmap[pageBitmapIndex] == true) continue;

        // Lock page if it is free
        LockPage((void*)(pageBitmapIndex * 4096));      

        // Return memory address of free page frame 
        return (void*)(pageBitmapIndex * 4096);       
    }


    return NULL; // Page Frame swap to file
}

void PageFrameAllocator::FreePages(void* address, uint64_t pageCount) {
    for (int t = 0; t < pageCount; t++) {
        FreePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::LockPage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true) return;     
    if (PageBitmap.Set(index, true)) {               
    freeMemory -= 4096;
    usedMemory += 4096;
    }
}

void PageFrameAllocator::LockPages(void* address, uint64_t pageCount) {
    for (int t = 0; t < pageCount; t++) {
        LockPage((void*)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::UnreservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;

    // Check if a pageframe is in use
    if (PageBitmap[index] == false) return;  

    // If it isn't then increase freememory, decrease usedmemory   
    if (PageBitmap.Set(index, false)) {         
            freeMemory += 4096;
            reservedMemory -= 4096;
            if (pageBitmapIndex > index) pageBitmapIndex = index;
    }
}

void PageFrameAllocator::UnreservePages(void* address, uint64_t pageCount) {
    for (int t = 0; t < pageCount; t++) {
        UnreservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void PageFrameAllocator::ReservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true) return;     
    if (PageBitmap.Set(index, true)) {               
        freeMemory -= 4096;
        reservedMemory += 4096;
    }
}



void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount) {
    for (int t = 0; t < pageCount; t++) {
        ReservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

    uint64_t PageFrameAllocator::GetFreeRAM() {
        return freeMemory;
    }
    uint64_t PageFrameAllocator::GetUsedRAM() {
        return usedMemory;
    }
    uint64_t PageFrameAllocator::GetReservedRAM() {
        return reservedMemory;
    }