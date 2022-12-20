#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

// Used for getting framebuffer info for passing to the kernel
typedef struct {
	void* BaseAddress;				// Base address of framebuffer
	size_t BufferSize;				// Size of framebuffer
	unsigned int Width;				// Horizontal resolution
	unsigned int Height;			// Vertical resolution
	unsigned int PixelsPerScanLine;	// Used for when framebuffer has more pixels than Width
} Framebuffer;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

// Structs used for loading a font
typedef struct {
	unsigned char magic[2];	// For checking a psf file, declared on lines 16 and 17
	unsigned char mode;		// Mode the font is in
	unsigned char charsize;	// How large characters are in bytes
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;	// Pointer to psf1_Header
	void* glyphBuffer;			// Pointner to array of glyphs
} PSF1_FONT;


// Create instance of framebuffer
Framebuffer framebuffer;

// Graphics output protocol - Initalizes graphics card to output a certain framebuffer to screen
Framebuffer* InitializeGOP(){
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;	// Find GOP GUID
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;						// Pointer to efi gop
	EFI_STATUS status;										// For checking failures

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);

	// Control for errors
	if(EFI_ERROR(status)){
		Print(L"Unable to locate GOP\n\r");
		return NULL;
	}
	else
	{
		Print(L"GOP located\n\r");
	}

	// Fill framebuffer entries
	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

	return &framebuffer;
	
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable){
	EFI_FILE* LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL){
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS){
		return NULL;
	}
	return LoadedFile;

}

// Function for loading a font into memory
PSF1_FONT* LoadPSF1Font(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	// Get file of the font
	EFI_FILE* font = LoadFile(Directory, Path, ImageHandle, SystemTable);

	// Control the existance of font
	if (font == NULL) return NULL;

	// Declare font header
	PSF1_HEADER* fontHeader;

	// Allocate memory for font header
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);

	// Read header into memory
	font->Read(font, &size, fontHeader);

	// Check if font is a valid file
	if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1){
		return NULL;
	}

	// Get size of buffer for glyph information
	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if (fontHeader->mode == 1) { //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	// Read info into glyphbuffer
	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));

		// Allocate memory for glyphbuffer
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);

		// Read glyphbuffer into memory
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	// Produce finished font
	PSF1_FONT* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);

	// Set entries in psf1 struct
	finishedFont->psf1_Header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont;

}

// 
int memcmp(const void* aptr, const void* bptr, size_t n){
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++){
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}


// Struct to make handing to the kernel easier 
typedef struct {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	UINTN mMapSize;
	UINTN mMapDescSize;
	void* rsdp;
} BootInfo;

// String compare function
UINTN strcmp(CHAR8* a, CHAR8* b, UINTN length) {
	for (UINTN i = 0; i < length; i++) {
		if (*a != *b) return 0;	// Compare failed
	}
	return 1;
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable);
	Print(L"String blah blah blah \n\r");

	EFI_FILE* Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);
	if (Kernel == NULL){
		Print(L"Could not load kernel \n\r");
	}
	else{
		Print(L"Kernel Loaded Successfully \n\r");
	}

	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		Kernel->Read(Kernel, &size, &header);
	}

	if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	)
	{
		Print(L"kernel format is bad\r\n");
	}
	else
	{
		Print(L"kernel header successfully verified\r\n");
	}

	Elf64_Phdr* phdrs;
	{
		Kernel->SetPosition(Kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		Kernel->Read(Kernel, &size, phdrs);
	}

	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
	)
	{
		switch (phdr->p_type){
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				Kernel->SetPosition(Kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				Kernel->Read(Kernel, &size, (void*)segment);
				break;
			}
		}
	}

	Print(L"Kernel Loaded\n\r");
	
	// Load and check for font zap-light16.psf
	PSF1_FONT* newFont = LoadPSF1Font(NULL, L"zap-light16.psf", ImageHandle, SystemTable);
	if (newFont == NULL){
		Print(L"Font is not valid or is not found\n\r");
	}
	else
	{
		Print(L"Font found. char size = %d\n\r", newFont->psf1_Header->charsize);
	}
	
	// Initalize graphics output protocol
	Framebuffer* newBuffer = InitializeGOP();

	// Print framebuffer entries
	Print(L"Base: 0x%x\n\rSize: 0x%x\n\rWidth: %d\n\rHeight: %d\n\rPixelsPerScanline: %d\n\r", 
	newBuffer->BaseAddress, 
	newBuffer->BufferSize, 
	newBuffer->Width, 
	newBuffer->Height, 
	newBuffer->PixelsPerScanLine);

	EFI_MEMORY_DESCRIPTOR* Map = NULL; 	// Pointer to the memory descriptor struct (Pages, Address, Type)
	UINTN MapSize, MapKey; 				// Size of maps and key for UEFI
	UINTN DescriptorSize;				// Size of descriptor entries
	UINT32 DescriptorVersion;
	{
		// Get the memory map and allocate memory for it
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

	}

	// Get rsdp
	EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	void* rsdp = NULL;
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;

	// Loop thorugh configuration table and find ACPI 20
	for (UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++) {
		if (CompareGuid(&configTable[index].VendorGuid, &Acpi2TableGuid)) {

			// Found an ACPI2 table
			if (strcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable, 8)) 
			{

				rsdp = (void*)configTable->VendorTable;
				//break;

			}

		}

		configTable++;

	}

	// Hand over the memory map in UEFI memory to the kernel
	void (*KernelStart)(BootInfo*) = ((__attribute__((sysv_abi)) void (*)(BootInfo*) ) header.e_entry);

	// Load info into the bootInfo struct
	BootInfo bootInfo;
	bootInfo.framebuffer = newBuffer;
	bootInfo.psf1_Font = newFont;
	bootInfo.mMap = Map;
	bootInfo.mMapSize = MapSize;
	bootInfo.mMapDescSize = DescriptorSize;
	bootInfo.rsdp = rsdp;

	/* 	ExitBootServices for Efi runtime cancellation and watchdog timer (to stop them from terminating)
		and to get access to system resources used by UEFI Fronttime */
	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	// Execute KernelStart in kernel.cpp
	KernelStart(&bootInfo);

	return EFI_SUCCESS; // Exit the UEFI application
}
