#include <iostream>
#include <fstream>

#include "OTTCharacterMap.hpp"
#include "OTTWindow.hpp"

OTTCharacterMap::OTTCharacterMap(const std::string& fname) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(8),
	nHeight(16),
	nLength(nWidth * nHeight),
	foreground(Colors::WHITE),
	background(Colors::BLACK),
	cmap(128, OTTBitmap(nWidth, nHeight, 1, 0x0))
{
	if (!loadCharacterBitmaps(fname)) {
		std::cout << " [OTTCharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
}

OTTCharacterMap::OTTCharacterMap(const std::string& fname, const unsigned short& W, const unsigned short& H) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(W),
	nHeight(H),
	nLength(nWidth * nHeight),
	foreground(Colors::WHITE),
	background(Colors::BLACK),
	cmap(128, OTTBitmap(nWidth, nHeight, 1, 0x0))
{
	if (!loadCharacterBitmaps(fname)) {
		std::cout << " [OTTCharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
}

OTTCharacterMap::OTTCharacterMap(
	const std::string& fname,
	const unsigned short& W,
	const unsigned short& H,
	const unsigned short& bitsPerPixel,
	OTTBitmap::bitmapDecodeFunction func) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(W),
	nHeight(H),
	nLength(nWidth* nHeight),
	foreground(Colors::WHITE),
	background(Colors::BLACK),
	cmap(128, OTTBitmap(nWidth, nHeight, 1, 0x0))
{
	if (!loadCharacterBitmaps(fname, bitsPerPixel, func)) {
		std::cout << " [OTTCharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
}

bool OTTCharacterMap::loadCharacterBitmaps(const std::string& fname, const unsigned short& nBitsPerPixel/* = 1*/, OTTBitmap::bitmapDecodeFunction func/* = 0x0*/) {
	// Load the ascii character map
	const size_t nBytesPerCharacter = nHeight * (nWidth / 8) * nBitsPerPixel;
	std::vector<unsigned char> rgb(nBytesPerCharacter, 0);
	std::ifstream ifile(fname.c_str(), std::ios::binary);
	//ifile.seekg(4, std::ios::beg);
	if (!ifile.good())
		return false;
	for (unsigned short x = 0; x < 128; x++) {
		ifile.read(reinterpret_cast<char*>(rgb.data()), nBytesPerCharacter); // Read all bytes for a single character
		if (ifile.eof()) {
			ifile.close();
			return false;
		}
		if(!func) // Normal bitmap decoder
			cmap[x].set(rgb.data());
		else // Externally defined bitmap decoder function
			cmap[x].set(rgb.data(), func);
	}
	ifile.close();
	return true;
}

void OTTCharacterMap::putCharacter(const char& val, const unsigned short& x, const unsigned short& y) {
	unsigned char pixelColor;
	for (unsigned short dy = 0; dy < nHeight; dy++) { // Rows
		for (unsigned short dx = 0; dx < nWidth; dx++) { // Columns
			pixelColor = cmap[(unsigned int)val].get(dx, dy);
			if (bInvertColors)
				pixelColor = 3 - pixelColor;
			if (bTransparency && pixelColor == 0) // Blank
				continue;
			window->buffWrite(
				nWidth * x + dx,
				nHeight * y + dy,
				(pixelColor > 0 ? foreground : background) //palette[pixelColor]
			);
		}
	}
}

void OTTCharacterMap::putString(const std::string& str, const unsigned short& x, const unsigned short& y, bool wrap/*=true*/) {
	unsigned short sx = x;
	unsigned short sy = y;
	for (size_t i = 0; i < str.length(); i++) {
		putCharacter(str[i], sx++, sy);
		if (sx >= 20) {
			if (!wrap)
				return;
			sx = 0;
			sy++;
		}
	}
}

void OTTCharacterMap::drawString(
	const std::string& str,
	const unsigned short& x0,
	const unsigned short& y0,
	OTTImageBuffer* buffer,
	const unsigned char& alphaColor/* = 4*/,
	bool invert/* = false*/
) {
	unsigned short sx = x0;
	unsigned char pixelColor;
	for (size_t i = 0; i < str.length(); i++) {
		for (unsigned short dy = 0; dy < nHeight; dy++) {
			for (unsigned short dx = 0; dx < nWidth; dx++) {
				pixelColor = cmap[(unsigned int)str[i]].get(dx, dy);
				if (invert)
					pixelColor = 3 - pixelColor;
				if (pixelColor == alphaColor) // Transparent
					continue;
				buffer->setPixel(
					sx + dx,
					y0 + dy,
					(pixelColor > 0 ? foreground : background) //palette[pixelColor]
				);
			}
		}
		sx += nWidth;
	}
}