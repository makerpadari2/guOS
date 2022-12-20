#pragma once
#include <stdint.h>

/*
Header file used in cstr.cpp and kernelUtil.h
*/

// Unsigned 64 bit Integer to string
const char* to_string(uint64_t value);   

// 64 bit Integer to string
const char* to_string(int64_t value);         

// 64 bit to Hexadecimal
const char* to_hstring(uint64_t value);        

// 32 bit to Hexadecimal
const char* to_hstring(uint32_t value);   

// 16 bit to Hexadecimal
const char* to_hstring(uint16_t value);     

// 8 bit to Hexadecimal
const char* to_hstring(uint8_t value); 

// Double value to string
const char* to_string(double value, uint8_t decimalPlaces); 

// Double value to string helper function
const char* to_string(double value);                        
