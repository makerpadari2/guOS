#include "interrupts.h"
#include "../panic.h"
#include "../IO.h"
#include "../userinput/keyboard.h"

/*
Interrupt will display a message upon an error, also handles keyboard input
*/


__attribute__((interrupt)) void PageFault_handler(interrupt_frame* frame)
{
    Panic("Page Fault Detected");
    while (true);
}

__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame)
{
    Panic("Double Fault Detected");
    while (true);
}

__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame)
{
    Panic("General Protection Fault Detected");
    while (true);
}

// Execute code upon keyboard input
__attribute__((interrupt)) void KeyboardINT_handler(interrupt_frame* frame)
{

    uint8_t scancode = inb(0x60); // 0x60 - port where keyboard is

    HandleKeyboard(scancode);

    PIC_EndMaster();

}

__attribute__((interrupt)) void MouseINT_handler(interrupt_frame* frame)
{

    uint8_t mouseData = inb(0x60); // 0x60 - port where keyboard is

    HandlePS2Mouse(mouseData);

    PIC_EndSlave();

}

void PIC_EndMaster() {
    outb(PIC1_COMMAND, PIC_EOI);
}

// Function for ending interrupt on PIC chip
void PIC_EndSlave(){
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void RemapPIC(){

    // Use values defined in interrupts.h to talk to the PIC chip and remap it
    // to avoid collisions with interrupts

    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    // Initalize master PIC chip
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // Initialize slave PIC chip
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // Tell PIC chips their interrupt offsets to stop them from colliding with
    // exceptions
    outb(PIC1_DATA, 0X20);
    io_wait();
    outb(PIC2_DATA, 0X28);
    io_wait();

    // Tell PIC chips how they correspond to each other
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    // Tell PIC chips to operate in 8086 mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Save datamask declared at the beginning of this function
    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);

}