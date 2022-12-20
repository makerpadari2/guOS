#pragma once

/*
Header file used in BasicRenderer.h
*/

struct PSF1_HEADER{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} ;

struct PSF1_FONT{
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} ;