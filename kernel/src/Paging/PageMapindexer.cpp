#include "PageMapindexer.h"

/*
The page map indexer converts a virtual memory class into a needed page map
*/

PageMapIndexer::PageMapIndexer(uint64_t virtualAddress) {

    // Bit shift to right because all addresses in index are 12 bit aligned
    virtualAddress >>= 12;

    // & with 0x1ff to get the part of virtualAddress that corresponds to P_i
    P_i = virtualAddress & 0x1ff;

    virtualAddress >>= 9;
    PT_i = virtualAddress & 0x1ff;

    virtualAddress >>= 9;
    PD_i = virtualAddress & 0x1ff;

    virtualAddress >>= 9;
    PDP_i = virtualAddress & 0x1ff;
}