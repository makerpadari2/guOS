#include "IO.h"

/*
Used for speaking to IO ports (and remapping PIC chip)
*/

void outb(uint16_t port, uint8_t value) {

    // Pass value to register %0 and port to register %1
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));

}

uint8_t inb(uint16_t port){
    uint8_t returnVal;

    // Put value from %0 to returnVal and value from %1 to port
    asm volatile ("inb %1, %0"
    : "=a"(returnVal)
    : "Nd"(port));
    return returnVal;
}

void io_wait() {

    // Send 0 port 0x80
    asm volatile ("outb %%al, $0x80" : : "a"(0));

}