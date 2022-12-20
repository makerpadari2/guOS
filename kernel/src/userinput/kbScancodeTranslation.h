#pragma once
#include <stdint.h>

// Use namespace to separate this table from other lookup tables
namespace QWERTYKeyboard {

    #define LeftShift 0x2A
    #define RightShift 0x36
    #define Enter 0x1C
    #define BackSpace 0x0E
    #define Spacebar 0x39

    extern const char ASCIITable[];

    // Take characters from ASCIITable and turn them uppercase
    char Translate(uint8_t scancode, bool uppercase);

}