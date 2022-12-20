#pragma once
#include <stddef.h>

/*
Header file for Framebuffer.cpp and BasicRenderer.h
*/

struct Framebuffer{
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
};
