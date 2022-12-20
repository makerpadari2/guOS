#pragma once
#include <stddef.h>
#include <stdint.h>

/*
Header file used in Bitmap.cpp, PageFrameAllocator.h and kernelUtil.h
*/

// Used to map a domain to bits for page frame allocation
class Bitmap{
    public:

    // Size of the bitmap
    size_t Size;   

    // Pointer to the start of the buffer                         
    uint8_t* Buffer;        

    // For getting a value at a specific index                
    bool operator[](uint64_t index); 

    // Set the value of the bit       
    bool Set(uint64_t index, bool value);   
};