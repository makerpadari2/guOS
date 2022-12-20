#pragma once
#include "paging.h"

/*
Header file used in PageTableManager.h
*/

class PageTableManager {
    public:
    PageTableManager(PageTable* PML4Address);    

    // Ponter to store value in PML4Address       
    PageTable* PML4;

    // Function for mapping virutal and physical memory
    void MapMemory(void* virtualMemory, void* physicalMemory);
};

extern PageTableManager g_PageTableManager;