#include <iostream>
#include <fstream>

#include "OTTCharacterMap.hpp"
#include "OTTWindow.hpp"

void getSmoothSpectrum(std::vector<ColorRGB>& palette, const unsigned short& nColors, const ColorRGB& startColor, const ColorRGB& endColor) {
	float fColor[3] = {
		startColor.r / 255.f,
		startColor.g / 255.f,
		startColor.b / 255.f
	}; // Starting color components
	float fDelta[3] = {
		(endColor.r / 255.f - fColor[0]) / nColors,
		(endColor.g / 255.f - fColor[1]) / nColors,
		(endColor.b / 255.f - fColor[2]) / nColors
	}; // Delta color components
	palette.clear();
	palette.reserve(nColors);
	for (unsigned short i = 0; i < nColors; i++) {
		palette.push_back(ColorRGB(fColor[0], fColor[1], fColor[2]));
		for (unsigned int j = 0; j < 3; j++)
			fColor[j] += fDelta[j];
	}
}

OTTCharacterMap::OTTCharacterMap(const std::string& fname) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(8),
	nHeight(16),
	nLength(nWidth * nHeight),
	nBitsPerPixel(1),
	nColorsPerPixel(2),
	palette{ Colors::BLACK, Colors::WHITE }, // background, foreground
	cmap(128, OTTBitmap(nWidth, nHeight, 1, 0x0))
{
	std::ifstream ifile(fname.c_str(), std::ios::binary);
	if (!ifile.good() || !loadCharacterBitmaps(ifile)) {
		std::cout << " [OTTCharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
	ifile.close();
}

OTTCharacterMap::OTTCharacterMap(const std::string& fname, const unsigned short& W, const unsigned short& H) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(W),
	nHeight(H),
	nLength(nWidth * nHeight),
	nBitsPerPixel(1),
	nColorsPerPixel(2),
	palette{ Colors::BLACK, Colors::WHITE }, // background, foreground
	cmap(128, OTTBitmap(nWidth, nHeight, 1, 0x0))
{
	std::ifstream ifile(fname.c_str(), std::ios::binary);
	if (!ifile.good() || !loadCharacterBitmaps(ifile)) {
		std::cout << " [OTTCharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
	ifile.close();
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
	nBitsPerPixel(bitsPerPixel),
	nColorsPerPixel((unsigned short)std::pow(2, bitsPerPixel)),
	palette(),
	cmap(128, OTTBitmap(nWidth, nHeight, 1, 0x0))
{
	// Generate smooth color spectrum
	getSmoothSpectrum(palette, nColorsPerPixel, Colors::BLACK, Colors::WHITE);

	// Load input file containing character bitmaps
	std::ifstream ifile(fname.c_str(), std::ios::binary);
	if (!ifile.good() || !loadCharacterBitmaps(ifile, func)) {
		std::cout << " [OTTCharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
	ifile.close();
}

OTTCharacterMap::OTTCharacterMap(const unsigned short& W, const unsigned short& H, const unsigned short& bitsPerPixel) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(W),
	nHeight(H),
	nLength(nWidth* nHeight),
	nBitsPerPixel(bitsPerPixel),
	nColorsPerPixel((unsigned short)std::pow(2, bitsPerPixel)),
	palette(),
	cmap(128, OTTBitmap(nWidth, nHeight, 1, 0x0))
{
	// Generate smooth color spectrum
	getSmoothSpectrum(palette, nColorsPerPixel, Colors::BLACK, Colors::WHITE);
}

bool OTTCharacterMap::loadCharacterBitmaps(std::ifstream& ifile, OTTBitmap::bitmapDecodeFunction func/* = 0x0*/) {
	// Load the ascii character map
	const size_t nBytesPerCharacter = nHeight * (nWidth / 8) * nBitsPerPixel;
	std::vector<unsigned char> rgb(nBytesPerCharacter, 0);
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
	return true;
}

void OTTCharacterMap::drawString(
	const std::string& str,
	const unsigned short& x0,
	const unsigned short& y0,
	OTTImageBuffer* buffer,
	const unsigned char& alphaColor/* = 0xff*/,
	bool invert/* = false*/
) {
	unsigned short sx = x0;
	unsigned char pixelColor;
	for (size_t i = 0; i < str.length(); i++) {
		for (unsigned short dy = 0; dy < nHeight; dy++) {
			for (unsigned short dx = 0; dx < nWidth; dx++) {
				pixelColor = cmap[(unsigned int)str[i]].get(dx, dy);
				if (invert)
					pixelColor = (nColorsPerPixel - 1) - pixelColor;
				if (pixelColor == alphaColor) // Transparent
					continue;
				buffer->setPixel(
					sx + dx,
					y0 + dy,
					palette[pixelColor]
				);
			}
		}
		sx += nWidth;
	}
}