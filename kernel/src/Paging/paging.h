#pragma once

#include <stdint.h>

/*
Header file used in paging.cpp and PageTableManager.h
*/

// Enum flags used instead of booleans (seen in comments below)
enum PT_Flag {
    Present = 0,
    ReadWrite = 1,
    UserSuper = 2,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
    LargerPages = 7,
    Custom0 = 9,
    Custom1 = 10,
    Custom2 = 11,
    NX = 63        // Only if supported
};

struct PageDirectoryEntry {

    uint64_t Value;
    void SetFlag(PT_Flag flag, bool enabled);

    // Used to check if flag is set or not
    bool GetFlag(PT_Flag flag);
    
    void SetAddress(uint64_t address);
    uint64_t GetAddress();

    /*
    // Attributes of page directory
    bool Present : 1;            // Page that the page directory is pointing to
    bool ReadWrite : 1;         // Check if page can be read or written to
    bool UserSuper : 1;         // Can the page be accessed by the user or superviser
    bool WriteThrough : 1;
    bool CacheDisabbled : 1;
    bool Accessed : 1;          // Used with CPU
    bool ignore0 : 1;       
    bool LargerPages : 1;       // Turns page directory to size of directory
    bool ignore1 : 1;
    uint8_t Available: 3;
    uint64_t Address : 40;
    */
};

// Hold 512 pages
struct PageTable {
    PageDirectoryEntry entries [512];
}__attribute__((aligned(0x1000)));