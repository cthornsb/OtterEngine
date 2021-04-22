#include <iostream>
#include <fstream>
#include <string>

#include "OTTWindow.hpp"
#include "OTTBitmap.hpp"

OTTBitmap::OTTBitmap() :	
	bBlank(true),
	nWidth(8),
	nHeight(8),
	nBits(0),
	pixels()
{
}

OTTBitmap::OTTBitmap(const int& W, const int& H, const int& bits, unsigned char* bmp) :
	bBlank(true),
	nWidth(W),
	nHeight(H),
	nBits(bits),
	pixels(W * H, 0)
{
}

unsigned char OTTBitmap::get(const unsigned short& x, const unsigned short& y) const {
	return pixels[y * nWidth + x];
}

void OTTBitmap::set(const unsigned short& x, const unsigned short& y, const unsigned char& color) {
	if (color != 0)
		bBlank = false;
	pixels[y * nWidth + x] = color;
}

void OTTBitmap::set(const unsigned char* data) {
	for (unsigned short i = 0; i < nHeight; i++) { // Over rows
		for (unsigned short j = 0; j < nWidth; j++) { // Over cols
			set(j, i, (data[i] & (0x1 << (nWidth - 1 - j))) == (0x1 << (nWidth - 1 - j)));
		}
	}
}

void OTTBitmap::set(const unsigned char* data, bitmapDecodeFunction decoder) {
	for (unsigned short i = 0; i < nHeight; i++) { // Over rows
		for (unsigned short j = 0; j < nWidth; j++) { // Over cols
			set(j, i, (*decoder)(j, i, data));
		}
	}
}

void OTTBitmap::dump() {
	for (unsigned short j = 0; j < nHeight; j++) { // Over rows
		for (unsigned short k = 0; k < nWidth; k++) { // Over cols
			std::cout << (int)pixels[j * nWidth + k];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
