#pragma once

#include <stdint.h>
#include "BasicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "Bitmap.h"
#include "Paging/PageFrameAllocator.h"
#include "Paging/PageMapindexer.h"
#include "Paging/paging.h"
#include "Paging/PageTableManager.h"
#include "userinput/mouse.h"
#include "acpi.h"
#include "pci.h"

/*
Header file for kernelUtil.cpp and kernel.cpp
*/

struct BootInfo {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
	ACPI::RSDP2* rsdp;
} ;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo {
    PageTableManager* pageTableManager;
};

// Return instance of KernelInfo
KernelInfo InitializeKernel(BootInfo* BootInfo);