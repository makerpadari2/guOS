#include "PageTableManager.h"
#include "PageMapindexer.h"
#include <stdint.h>
#include "PageFrameAllocator.h"
#include "../memory.h"

/*
The page table manager is used for mapping physical memory to virtual memory
*/

// Global page table manager (used in PCI)
PageTableManager g_PageTableManager = NULL;

// Implementation for constructor
PageTableManager::PageTableManager(PageTable* PML4Address) {
    this->PML4 = PML4Address;
}

void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory) {
    // Get page map indexes
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);

    // Hold page directory of each page table
    PageDirectoryEntry PDE;

    // Get page directory pointer from PML4 (Page map level 4)
    PDE = PML4->entries[indexer.PDP_i];

    // Pointer to page table
    PageTable* PDP;

    // Check if page directory pointer is present
    if (!PDE.GetFlag(PT_Flag::Present)) {
        PDP = (PageTable*)GlobalAllocator.RequestPage();

        // Set all bytes to 0
        memset(PDP, 0, 0x1000); // Size of 1 page is 4096
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);

        // Set entry in PLM4 to new page entry that was made
        PML4->entries[indexer.PDP_i] = PDE;
    } else {
        // Get address stored in PDE and bitshift left by 
        // 12 bits and assign it to PDP
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }

    // Get page directory from PDP
    PDE = PDP->entries[indexer.PD_i];

    // Pointer to page table
    PageTable* PD;

    // Check if page directory is present
    if (!PDE.GetFlag(PT_Flag::Present)) {
        PD = (PageTable*)GlobalAllocator.RequestPage();

        // Set all bytes to 0
        memset(PD, 0, 4096); // Size of 1 page is 4096
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);

        // Set entry in PDP to new page entry that was made
        PDP->entries[indexer.PD_i] = PDE;
    } else {
        // Get address stored in PDE and bitshift left by 
        // 12 bits and assign it to PDP
        PD = (PageTable*)((uint64_t)PDE.GetAddress()  << 12);
    }

    // Get page table from PD
    PDE = PD->entries[indexer.PT_i];

    // Pointer to page table
    PageTable* PT;

    // Check if page directory pointer is present
    if (!PDE.GetFlag(PT_Flag::Present)) {
        PT = (PageTable*)GlobalAllocator.RequestPage();

        // Set all bytes to 0
        memset(PT, 0, 4096); // Size of 1 page is 4096
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);

        // Set entry in PD to new page entry that was made
        PD->entries[indexer.PT_i] = PDE;
    } else {
        // Get address stored in PDE and bitshift left by 
        // 12 bits and assign it to PDP
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }

    // Get the page from page table
    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PT->entries[indexer.P_i] = PDE;

}