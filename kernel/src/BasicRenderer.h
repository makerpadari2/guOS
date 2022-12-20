#pragma once
#include "math.h"
#include "Framebuffer.h"
#include "simpleFonts.h"
#include <stdint.h>

/*
Header file used in BasicRenderer.cpp, Interrupts/interrupts.h and kernelUtil.h
*/

class BasicRenderer{
    public:
    BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font);
    Point CursorPosition;
    Framebuffer* TargetFramebuffer;
    PSF1_FONT* PSF1_Font;

    // Used for removing old cursor everytime cursor is moved
    uint32_t MouseCursorBuffer[16 * 16];

    uint32_t MouseCursorBufferAfter[16 * 16];

    unsigned int Colour;
    unsigned int ClearColour;
    void Print(const char* str);
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);
    void PutChar(char chr);
    void PutPix(uint32_t x, uint32_t y, uint32_t color);
    uint32_t GetPix(uint32_t x, uint32_t y);

    // Used for keyboard backspace
    void ClearChar(); 

    // Clear function to set entire screen to single color
    void Clear();

    // Next function - new line
    void Next();

    void DrawOverlayMouseCursor(uint8_t* mouseCursor, Point position, 
    uint32_t color);

    void ClearMouseCursor(uint8_t* mouseCursor, Point position);

    bool MouseDrawn;

    
};

// Global renderer for page fault
extern BasicRenderer* GlobalRenderer;