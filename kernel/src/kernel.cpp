#include "kernelUtil.h"

/*
Exectued kernel from gnu-efi/bootloader/main.c
*/

extern "C" void _start(BootInfo* bootInfo){
    
    // Initialize the kernel with the bootInfo (kernelUtil.h)
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;



    GlobalRenderer->Print("Kernel Initialized Successfully");

    GlobalRenderer->Next();
    GlobalRenderer->Print(to_hstring((uint64_t)bootInfo->rsdp));

    // Read mouse port
    while (true) {
        ProcessMousePacket();
    }

    while(true);
}