#include "BasicRenderer.h"

/*
Graphics functions for printing to screen
*/

BasicRenderer* GlobalRenderer;

BasicRenderer::BasicRenderer(Framebuffer* targetFramebuffer, 
PSF1_FONT* psf1_Font)
{
    TargetFramebuffer = targetFramebuffer;
    PSF1_Font = psf1_Font;
    Colour = 0xffffffff;
    CursorPosition = {0, 0};
}

void BasicRenderer::PutPix(uint32_t x, uint32_t y, uint32_t color) {

    // Calculate pixel address and set it into color
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) +
     (y * TargetFramebuffer->PixelsPerScanLine * 4)) = color;

}

uint32_t BasicRenderer::GetPix(uint32_t x, uint32_t y) {

    return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) +
     (y * TargetFramebuffer->PixelsPerScanLine * 4));

}

void BasicRenderer::ClearMouseCursor(uint8_t* mouseCursor, Point position) {

    if (!MouseDrawn) return;

    // Make sure cursor doesn't draw outside of framebuffer
    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;


    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {

            // Get bit of bitmap
            int bit = y * 16 + x;

            // Get byte of bitmap
            int byte = bit / 8;

            // Check if bit is set
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {   

                if (GetPix(position.X + x, position.Y + y) == 
                MouseCursorBufferAfter[x + y * 16]) 
                {
                    PutPix(position.X + x, position.Y + y, 
                    MouseCursorBuffer[x + y * 16]);
                }

            }
        }
    }
    
}

// Draw mouse cursor
void BasicRenderer::DrawOverlayMouseCursor(
    uint8_t* mouseCursor, Point position, uint32_t color) 
    {

    // Make sure cursor doesn't draw outside of framebuffer
    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;


    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {

            // Get bit of bitmap
            int bit = y * 16 + x;

            // Get byte of bitmap
            int byte = bit / 8;

            // Check if bit is set
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {   

                // Copy contents from framebuffer into the mouse cursor buffer
                MouseCursorBuffer[x + y * 16] = 
                GetPix(position.X + x, position.Y + y);

                PutPix(position.X + x, position.Y + y, color);

                MouseCursorBufferAfter[x + y * 16] = 
                GetPix(position.X + x, position.Y + y);

            }
        }
    }

    MouseDrawn = true;

}

void BasicRenderer::Clear() {

    // Get base of framebuffer
    uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;

    // Get ammount of bytes per scanline
    uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;

    // Get height of framebuffer
    uint64_t fbHeight = TargetFramebuffer->Height;

    // Get size of framebuffer
    uint64_t fbSize = TargetFramebuffer->BufferSize;

    // Iterate through every single row in framebuffer and set it to color
    for (int verticalScanline = 0; verticalScanline < fbHeight; 
    verticalScanline ++) {

        // Point to first pixel in every row
        uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);

        for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < 
        (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++) {
            *pixPtr = ClearColour;
        }

    }
}

void BasicRenderer::ClearChar(){

    // Check if cursor position is out of bounds
    if (CursorPosition.X == 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

    unsigned int xOff = CursorPosition.X;
    unsigned int yOff = CursorPosition.Y;

    // Declare pixel pointer
    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;

    // Draw the character, select bit in bitmap
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff - 8; x < xOff; x++){

                    // If there is a pixel there
                    *(unsigned int*)(pixPtr + x + 
                    (y * TargetFramebuffer->PixelsPerScanLine)) = ClearColour;

        }
    }

    CursorPosition.X -= 8;

    if (CursorPosition.X < 0) {
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

}

void BasicRenderer::Next(){
    CursorPosition.X = 0;
    CursorPosition.Y += 16;
}

// Print string function
void BasicRenderer::Print(const char* str)
{
    
    char* chr = (char*)str;

    // While there are characters in string
    while(*chr != 0){

        // Display character
        PutChar(*chr, CursorPosition.X, CursorPosition.Y);

        // Move cursor to next position
        CursorPosition.X+=8;

        // Go to next line if cursor goes off screen
        if(CursorPosition.X + 8 > TargetFramebuffer->Width)
        {
            CursorPosition.X = 0;
            CursorPosition.Y += 16;
        }

        // Next character
        chr++;
    }
}

// Function for drawing characters to screen
void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
{
    // Declare pixel pointer
    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;

    /* Pointer to glyph in glyphbuffer (declared in gnu-efi/bootloader/main.c).
       Get base address of glyphbuffer and add on the character times 
       character size*/
    char* fontPtr = (char*)PSF1_Font->glyphBuffer + 
    (chr * PSF1_Font->psf1_Header->charsize);

    // Draw the character, select bit in bitmap
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){

            /* Check if bit in bitmap is on or off by shifting 
               a bit to the right by x - x offset*/
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){

                    // If there is a pixel there
                    *(unsigned int*)(pixPtr + x + 
                    (y * TargetFramebuffer->PixelsPerScanLine)) = Colour;
                }

        }
        fontPtr++;
    }
}

// Function for drawing characters to screen (for keyboard input)
void BasicRenderer::PutChar(char chr)
{
    // Put character on screen
    PutChar(chr, CursorPosition.X, CursorPosition.Y);

    // Move cusor
    CursorPosition.X += 8;

    // Check if character is out of bounds
    if (CursorPosition.X + 8 > TargetFramebuffer->Width) {
        CursorPosition.X = 0;
        CursorPosition.Y += 16;
    }
}