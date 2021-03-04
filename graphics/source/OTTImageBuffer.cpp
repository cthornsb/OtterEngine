#include <algorithm>

#include "OTTImageBuffer.hpp"

void OTTImageBuffer::resize(const unsigned short& W, const unsigned short& H){
	nWidth = W;
	nHeight = H;
	nSize = W * H;
	nBytes = nSize * 3;
	bitmap.resize(nBytes); // Contract size or expand, filling new pixels with zero
	dptr = &bitmap[0]; // Image data may have been moved
}

void OTTImageBuffer::setPixel(const unsigned short& x, const unsigned short& y, const ColorRGB& color){
	unsigned char* base = get(x, y);
	base[0] = color.r;
	base[1] = color.g;
	base[2] = color.b;
}

void OTTImageBuffer::setPixelRow(const unsigned short& y, const ColorRGB& color){
	for(unsigned short i = 0; i < nWidth; i++)
		setPixel(i, y, color);
}

void OTTImageBuffer::clear(const ColorRGB& color){
	for(unsigned int i = 0; i < nSize; i++){
		dptr[3 * i]     = color.r;
		dptr[3 * i + 1] = color.g;
		dptr[3 * i + 2] = color.b;
	}
}

void OTTImageBuffer::fill(const unsigned char& value/*=0*/){
	std::fill(bitmap.begin(), bitmap.end(), value);
}

void OTTImageBuffer::free(){
	bitmap.resize(0);
	dptr = 0x0;
}

