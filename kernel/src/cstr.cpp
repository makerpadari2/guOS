#include "cstr.h"

/*
    Convert numbers to strings (int, double and hex)
*/

// Buffer for characters
char uintTo_StringOutput[128];     

const char* to_string(uint64_t value){

    // Size of string
    uint8_t size;       

    // Used to find out how many characters are needed            
    uint64_t sizeTest = value;      
    while (sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;              // Buffer index
    while(value / 10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;

        // Put character into buffer
        uintTo_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintTo_StringOutput[size - index] = remainder + '0';

    // Null terminate the string
    uintTo_StringOutput[size + 1] = 0; 
    return uintTo_StringOutput;
}

char hexTo_StringOutput[128];           // Buffer for hexadecimal values
const char* to_hstring(uint64_t value){
    uint64_t* valPtr = &value;          // Pointer to the address of value
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;           // Size of hexadecimal for display

    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);

        // Isolate one side of the hex value (left side to the right by 4 bits)
        tmp = ((*ptr & 0xF0) >> 4);

        // Add to buffer
        hexTo_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');

        // Take right side of the hex
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput[size + 1] = 0;
    return hexTo_StringOutput;
}

// Hexadecimal (uint32_t) to string
char hexTo_StringOutput32[128];
const char* to_hstring(uint32_t value){
    uint32_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput32[size + 1] = 0;
    return hexTo_StringOutput32;
}

// Hexadecimal (uint16_t) to string
char hexTo_StringOutput16[128];
const char* to_hstring(uint16_t value){
    uint16_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput16[size + 1] = 0;
    return hexTo_StringOutput16;
}

// Hexadecimal (uint8_t) to string
char hexTo_StringOutput8[128];
const char* to_hstring(uint8_t value){
    uint8_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput8[size + 1] = 0;
    return hexTo_StringOutput8;
}

// Integer to string
char intTo_StringOutput[128];       // Buffer for negative values
const char* to_string(int64_t value){
    uint8_t isNegative = 0;

    if (value < 0){ 
        isNegative = 1;
        value *= -1;                    // Turn value into positive 
        intTo_StringOutput[0] = '-';    // Add a - to front
    }

    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;

        // isNegative moves all characters forward by one for the - symbol
        intTo_StringOutput[isNegative + size - index] = remainder + '0'; 
        index++;
    }
    uint8_t remainder = value % 10;
    intTo_StringOutput[isNegative + size - index] = remainder + '0';
    intTo_StringOutput[isNegative + size + 1] = 0; 
    return intTo_StringOutput;
}

// Double value to string

// Buffer for decimal values 
char doubleTo_StringOutput[128];                                
const char* to_string(double value, uint8_t decimalPlaces){
    if (decimalPlaces > 20) decimalPlaces = 20;                 // Error check

    // Get the decimal position
    char* intPtr = (char*)to_string((int64_t)value);            
    char* doublePtr = doubleTo_StringOutput;

    if (value < 0){
        value *= -1;
    }

    // Copy characters from int buffer to double buffer
    while(*intPtr != 0){
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    // Add a stop for decimal
    *doublePtr = '.';
    doublePtr++;

    // Isolate decimal portion
    double newValue = value - (int)value;           

    for (uint8_t i = 0; i < decimalPlaces; i++){
        newValue *= 10;
        *doublePtr = (int)newValue + '0';

        // Take integer portion off the newValue
        newValue -= (int)newValue;          
        doublePtr++;
    }


    // Finalize the string
    *doublePtr = 0;
    return doubleTo_StringOutput;
}


// doubleTo_StringOutput helper function (Specifies decimal places)
const char* to_string(double value){
    return to_string(value, 2);
}
