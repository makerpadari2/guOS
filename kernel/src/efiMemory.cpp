#include "efiMemory.h"
/*
EFI memory is used to find out which sections of memory are availabe for the 
kernel to use
*/


/* Memory type strings, adding all these together will equal to total memory 
allocated for the system */
const char* EFI_MEMORY_TYPE_STRINGS[] {

    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",

    // Memory section UEFI has not used that is free to use in the kernel
    "EfiConventionalMemory",    
    "EfiUnusableMemory",

    // Memory that can be used after getting info from ACPI tables
    "EfiACPIReclaimMemory",     
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode",

};