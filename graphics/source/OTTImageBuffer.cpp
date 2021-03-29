#include <algorithm>

#include "OTTImageBuffer.hpp"
#include "OTTLogicalColor.hpp"

bool OTTImageBuffer::getPixel(const int& x, const int& y, OTTLogicalColor& color) {
	if (x >= nWidth || y >= nHeight)
		return false;
	color = OTTLogicalColor(&dptr[(nWidth * y + x) * nChannels]);
	return true;
}

const unsigned char* OTTImageBuffer::getPixel(const int& x, const int& y) const {
	if (x >= nWidth || y >= nHeight)
		return 0x0;
	return &dptr[(nWidth * y + x) * nChannels];
}

void OTTImageBuffer::copyImageData(std::vector<unsigned char>& output) const {
	output = std::vector<unsigned char>(bitmap.cbegin(), bitmap.cend());
}

void OTTImageBuffer::resize(const unsigned short& W, const unsigned short& H){
	nWidth = W;
	nHeight = H;
	nSize = W * H;
	nBytes = nSize * nChannels;
	bitmap.resize(nBytes); // Contract size or expand, filling new pixels with zero
	dptr = &bitmap[0]; // Image data may have been moved
}

void OTTImageBuffer::setPixel(const unsigned short& x, const unsigned short& y, const ColorRGB& color){
	unsigned char* base = get(x, y);
	for(unsigned short i = 0; i < nChannels; i++)
		base[i] = color[i];
}

void OTTImageBuffer::setPixelRow(const unsigned short& y, const ColorRGB& color){
	for(unsigned short i = 0; i < nWidth; i++)
		setPixel(i, y, color);
}

void OTTImageBuffer::clear(const ColorRGB& color){
	for(unsigned int i = 0; i < nSize; i++){
		for(unsigned short j = 0; j < nChannels; j++)
			dptr[nChannels * i + j] = color[j];
	}
}

void OTTImageBuffer::fill(const unsigned char& value/*=0*/){
	std::fill(bitmap.begin(), bitmap.end(), value);
}

void OTTImageBuffer::free(){
	bitmap.resize(0);
	dptr = 0x0;
}

