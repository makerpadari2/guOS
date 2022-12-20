#include "kernelUtil.h"
#include "gdt/gdt.h"
#include "Interrupts/IDT.h"
#include "Interrupts/interrupts.h"
#include "IO.h"

/*
Functions are declared here to keep kernel.cpp clean
*/

// Create instance of KernelInfo
KernelInfo kernelInfo;

// Function for preparing memory (IE paging)
void PrepareMemory(BootInfo* bootInfo) {

    // Memory map entries
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    // Page Frame Allocator
    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize,
    bootInfo->mMapDescSize);

    /* Get size of the kernel, _KernelStart and _KernelEnd 
       defined in kernel.ld file*/
    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    // Used to make sure the kernel is locked away to avoid memory collisions
    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    // Create Page map level 4
    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0 , 0x1000);

    // Create Page Table Manager that takes the created PML4 as new address
    g_PageTableManager = PageTableManager(PML4);

    // Identity map entire span of memory
    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, 
    bootInfo->mMapDescSize); t += 0x1000) 
    {
        // Go through each page in memory and map it
        g_PageTableManager.MapMemory((void*)t, (void*)t);
    }

    // Check if framebuffer is still mapped in the address where it was

    // Framebuffer base
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;

    //Framebuffer size (add extra page with 0x1000 to not under allocate)
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;

    GlobalAllocator.LockPages((void*)fbBase, fbSize/ 0x1000 + 1);

    // Identity map framebuffer
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096) {
        g_PageTableManager.MapMemory((void*)t, (void*)t);
    }
   
    // Load PML4 to cr3 register so that the memory managment unit will start
    // using the new page map
    asm ("mov %0, %%cr3" : : "r" (PML4));

    kernelInfo.pageTableManager = &g_PageTableManager;
}

IDTR idtr;

// Function for creating interrupt entry descriptor tables
void SetIDTGate(void* handler, uint8_t entryOffset,  
uint8_t type_attr, uint8_t selector) 
{

    IDTDescEntry* interrupt = (IDTDescEntry*)(idtr.Offset + 
    entryOffset * sizeof(IDTDescEntry));


    interrupt->SetOffset((uint64_t)handler);

    // Set type and attributes of page fault entry
    interrupt->type_attr = type_attr;

    // Set code selector 
    interrupt->selector = selector;

}

void PrepareInterrupts(){

    // Set limit and offset of idtr
    idtr.Limit = 0x0FFF;
    // Allocate space to IDT
    idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

    // Force a fault, IDT offset = 0xE
    SetIDTGate((void*)PageFault_handler, 0xE, IDT_TA_InterruptGate, 0x08);

    // Double fault (Fault within page fault), IDT offset = 0x8
    SetIDTGate((void*)DoubleFault_handler, 0x8, IDT_TA_InterruptGate, 0x08);

    // General protection fault (Fault within page fault), IDT offset = 0xD
    SetIDTGate((void*)GPFault_handler, 0xD, IDT_TA_InterruptGate, 0x08);

    // Interrupt for keyboard
    SetIDTGate((void*)KeyboardINT_handler, 0x21, IDT_TA_InterruptGate, 0x08);

    // Interrupt for mouse
    SetIDTGate((void*)MouseINT_handler, 0x2C, IDT_TA_InterruptGate, 0x08);



    // Read new IDTR value to IDTR register
    asm("lidt %0" : : "m" (idtr));

    RemapPIC();

}

// Go through rsdp to find MCFG and other system descriptor tables
void PrepareACPI(BootInfo* bootInfo) {
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);

    ACPI::MCFGHeader* mcfg = 
    (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");

    // Print signature 
    for (int t = 0; t < 4; t++) {
        GlobalRenderer->PutChar(mcfg->Header.Signature[t]);
    }

    PCI::EnumeratePCI(mcfg);

}

// For intializing the global renderer
BasicRenderer r = BasicRenderer(NULL, NULL);

KernelInfo InitializeKernel(BootInfo* bootInfo) {

    // Initialize global renderer
    r = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_Font);
    GlobalRenderer = &r;

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;

    // Load the GDT
    LoadGDT(&gdtDescriptor);

    PrepareMemory(bootInfo);

    memset(bootInfo->framebuffer->BaseAddress, 0, 
    bootInfo->framebuffer->BufferSize);

    PrepareInterrupts();

    InitPS2Mouse();

    PrepareACPI(bootInfo);

    // Unmask secound interrupt
    outb(PIC1_DATA, 0b11111001);
    outb(PIC2_DATA, 0b11101111);

    // Enable maskable interrupts
    asm ("sti");

    return kernelInfo;
}