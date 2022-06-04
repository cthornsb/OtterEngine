#include "OTTBitmap.hpp"

#include <iostream>

ott::Bitmap::Bitmap() :	
	bBlank(true),
	nWidth(8),
	nHeight(8),
	nBits(0),
	pixels()
{
}

ott::Bitmap::Bitmap(const uint16_t& W, const uint16_t& H, const uint8_t& bits, uint8_t* bmp) :
	bBlank(true),
	nWidth(W),
	nHeight(H),
	nBits(bits),
	pixels(W * H, 0)
{
}

uint8_t ott::Bitmap::Get(const uint16_t& x, const uint16_t& y) const {
	return pixels[y * nWidth + x];
}

void ott::Bitmap::Set(const uint16_t& x, const uint16_t& y, const uint8_t& color) {
	if (color != 0)
		bBlank = false;
	pixels[y * nWidth + x] = color;
}

void ott::Bitmap::Set(const uint8_t* data) {
	for (uint16_t i = 0; i < nHeight; i++) { // Over rows
		for (uint16_t j = 0; j < nWidth; j++) { // Over cols
			this->Set(j, i, (data[i] & (0x1 << (nWidth - 1 - j))) == (0x1 << (nWidth - 1 - j)));
		}
	}
}

void ott::Bitmap::Set(const uint8_t* data, BitmapDecodeFunction_t decoder) {
	for (uint16_t i = 0; i < nHeight; i++) { // Over rows
		for (uint16_t j = 0; j < nWidth; j++) { // Over cols
			this->Set(j, i, (*decoder)(j, i, data));
		}
	}
}

void ott::Bitmap::Dump() {
	for (uint16_t j = 0; j < nHeight; j++) { // Over rows
		for (uint16_t k = 0; k < nWidth; k++) { // Over cols
			std::cout << (int)pixels[j * nWidth + k];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
