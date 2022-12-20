#pragma once
#include <stdint.h>

// port - For selecting device on IO bus to talk to, 
// value - Value for the device
void outb (uint16_t port, uint8_t value);

// Read value from device at the specific port
uint8_t inb(uint16_t port);

// Wait for completion
void io_wait();