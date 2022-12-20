#include "pci.h"

namespace PCI {
    // Go through entire table of PCI devices
    void EnumerateFunction(uint64_t deviceAddress, uint64_t function) {
        // Calculate offset of bus address from base address
        uint64_t offset = function << 12;

        uint64_t functionAddress = deviceAddress + offset;

        // Identitymap bus address to itself
        g_PageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);

        // Get PCI device header
        PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)functionAddress;

        // Invalid answers
        if (pciDeviceHeader->DeviceID == 0) return;
        if (pciDeviceHeader->DeviceID == 0xFFFF) return;

        GlobalRenderer->Print(to_hstring(pciDeviceHeader->VendorID));
        GlobalRenderer->Print("");
        GlobalRenderer->Print(to_hstring(pciDeviceHeader->DeviceID));
        GlobalRenderer->Next();
    }

    void EnumerateDevice(uint64_t busAddress, uint64_t device) {
        // Calculate offset of device address from bus address
        uint64_t offset = device << 15;

        uint64_t deviceAddress = busAddress + offset;

        // Identitymap bus address to itself
        g_PageTableManager.MapMemory((void*)deviceAddress, (void*)deviceAddress);

        // Get PCI device header
        PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)deviceAddress;

        // Invalid answers
        if (pciDeviceHeader->DeviceID == 0) return;
        if (pciDeviceHeader->DeviceID == 0xFFFF) return;

        for (uint64_t function = 0; function < 8; function++) {
            EnumerateFunction(deviceAddress, function);
        }
    }

    void EnumerateBus(uint64_t baseAddress, uint64_t bus) {
        // Calculate offset of bus address from base address
        uint64_t offset = bus << 20;

        uint64_t busAddress = baseAddress + offset;

        // Identitymap bus address to itself
        g_PageTableManager.MapMemory((void*)busAddress, (void*)busAddress);

        // Get PCI device header
        PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)busAddress;

        // Invalid answers
        if (pciDeviceHeader->DeviceID == 0) return;
        if (pciDeviceHeader->DeviceID == 0xFFFF) return;

        for (uint64_t device = 0; device < 32; device++) {
            EnumerateDevice(busAddress, device);
        }
    }

    void EnumeratePCI(ACPI::MCFGHeader* mcfg) {

        // Get ammount of entries in mcfg to get acpi device config
        int entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) /
        sizeof(ACPI::DeviceConfig);

    for (int t = 0; t <  entries; t++) {
        /*
        Get base address of mcfg and add size of mcfg to the end of it, add size
        of deviceconfig times t
        */
        ACPI::DeviceConfig* newDeviceConfig = (ACPI::DeviceConfig*)
        ((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) 
        * t));

        for (uint64_t bus = newDeviceConfig->StartBus; 
        bus < newDeviceConfig->EndBus;bus++) 
        {
            EnumerateBus(newDeviceConfig->BaseAddress, bus);
        }
    }

    }
}