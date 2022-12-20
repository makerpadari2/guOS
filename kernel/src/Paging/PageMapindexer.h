#pragma once
#include <stdint.h>

/*
Header file used in PageMapindexer.cpp and PageTableManager.cpp
*/

class PageMapIndexer {
    public:
        PageMapIndexer(uint64_t virtualAddress);
        uint64_t PDP_i;                             // Page directory pointer index
        uint64_t PD_i;                              // Page directory index
        uint64_t PT_i;                              // Page table index
        uint64_t P_i;                               // Page Index
};