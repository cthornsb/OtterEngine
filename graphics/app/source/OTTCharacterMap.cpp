#include "OTTCharacterMap.hpp"

#include <graphics/core/OTTWindow.hpp>

#include <iostream>

namespace ott {

	void getSmoothSpectrum(std::vector<ColorRGB>& palette, const uint16_t& nColors, const ColorRGB& startColor, const ColorRGB& endColor);

} /* namespace ott */

ott::CharacterMap::CharacterMap(const std::string& fname) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(8),
	nHeight(16),
	nLength(nWidth * nHeight),
	nBitsPerPixel(1),
	nColorsPerPixel(2),
	nCursorX(0),
	nCursorY(0),
	cursorAlignment(TextAlignment::TopLeft),
	palette{ colors::Black, colors::White }, // background, foreground
	cmap(128, Bitmap(nWidth, nHeight, 1, 0x0))
{
	std::ifstream ifile(fname.c_str(), std::ios::binary);
	if (!ifile.good() || this->LoadCharacterBitmaps(ifile) == false) {
		std::cout << " [CharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
	ifile.close();
}

ott::CharacterMap::CharacterMap(const std::string& fname, const uint16_t& W, const uint16_t& H) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(W),
	nHeight(H),
	nLength(nWidth * nHeight),
	nBitsPerPixel(1),
	nColorsPerPixel(2),
	palette{ colors::Black, colors::White }, // background, foreground
	cmap(128, Bitmap(nWidth, nHeight, 1, 0x0))
{
	std::ifstream ifile(fname.c_str(), std::ios::binary);
	if (!ifile.good() || this->LoadCharacterBitmaps(ifile) == false) {
		std::cout << " [CharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
	ifile.close();
}

ott::CharacterMap::CharacterMap(
	const std::string& fname,
	const uint16_t& W,
	const uint16_t& H,
	const uint16_t& bitsPerPixel,
	Bitmap::BitmapDecodeFunction_t func) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(W),
	nHeight(H),
	nLength(nWidth* nHeight),
	nBitsPerPixel(bitsPerPixel),
	nColorsPerPixel((uint16_t)std::pow(2, bitsPerPixel)),
	palette(),
	cmap(128, Bitmap(nWidth, nHeight, 1, 0x0))
{
	// Generate smooth color spectrum
	getSmoothSpectrum(palette, nColorsPerPixel, colors::Black, colors::White);

	// Load input file containing character bitmaps
	std::ifstream ifile(fname.c_str(), std::ios::binary);
	if (!ifile.good() || this->LoadCharacterBitmaps(ifile) == false) {
		std::cout << " [CharacterMap] Error! Failed to load character map file \"" << fname << "\"." << std::endl;
	}
	ifile.close();
}

ott::CharacterMap::CharacterMap(const uint16_t& W, const uint16_t& H, const uint16_t& bitsPerPixel) :
	window(0x0),
	bTransparency(false),
	bInvertColors(false),
	nWidth(W),
	nHeight(H),
	nLength(nWidth* nHeight),
	nBitsPerPixel(bitsPerPixel),
	nColorsPerPixel((uint16_t)std::pow(2, bitsPerPixel)),
	palette(),
	cmap(128, Bitmap(nWidth, nHeight, 1, 0x0))
{
	// Generate smooth color spectrum
	getSmoothSpectrum(palette, nColorsPerPixel, colors::Black, colors::White);
}

bool ott::CharacterMap::LoadCharacterBitmaps(std::ifstream& ifile, Bitmap::BitmapDecodeFunction_t func/* = 0x0*/) {
	// Load the ascii character map
	const size_t nBytesPerCharacter = nHeight * (nWidth / 8) * nBitsPerPixel;
	std::vector<uint8_t> rgb(nBytesPerCharacter, 0);
	if (!ifile.good())
		return false;
	for (uint16_t x = 0; x < 128; x++) {
		ifile.read(reinterpret_cast<char*>(rgb.data()), nBytesPerCharacter); // Read all bytes for a single character
		if (ifile.eof()) {
			ifile.close();
			return false;
		}
		if(!func) // Normal bitmap decoder
			cmap[x].Set(rgb.data());
		else // Externally defined bitmap decoder function
			cmap[x].Set(rgb.data(), func);
	}
	return true;
}

void ott::CharacterMap::DrawString(
	const std::string& str,
	const uint16_t& x0,
	const uint16_t& y0,
	ImageBuffer* buffer,
	const uint8_t& alphaColor/* = 0xff*/,
	bool invert/* = false*/
) {
	uint16_t sx = x0;
	uint8_t pixelColor;
	for (size_t i = 0; i < str.length(); i++) {
		for (uint16_t dy = 0; dy < nHeight; dy++) {
			for (uint16_t dx = 0; dx < nWidth; dx++) {
				pixelColor = cmap[(uint32_t)str[i]].Get(dx, dy);
				if (invert)
					pixelColor = (nColorsPerPixel - 1) - pixelColor;
				if (pixelColor == alphaColor) // Transparent
					continue;
				buffer->SetPixel(
					sx + dx,
					y0 + dy,
					palette[pixelColor]
				);
			}
		}
		sx += nWidth;
	}
}

void ott::CharacterMap::DrawString(
	const std::string& str,
	ImageBuffer* buffer,
	const uint8_t& alphaColor/* = 0xff*/,
	bool invert/* = false*/
) {
	uint16_t px = nCursorX;
	uint16_t py = nCursorY;
	switch (cursorAlignment) {
	case TextAlignment::TopLeft:
		break;
	case TextAlignment::TopCenter:
		px -= this->StrWidthPixels(str) / 2;
		break;
	case TextAlignment::TopRight:
		px -= this->StrWidthPixels(str);
		break;
	case TextAlignment::MiddleLeft:
		py -= nHeight / 2;
		break;
	case TextAlignment::MiddleCenter:
		px -= this->StrWidthPixels(str) / 2;
		py -= nHeight / 2;
		break;
	case TextAlignment::MiddleRight:
		px -= this->StrWidthPixels(str);
		py -= nHeight / 2;
		break;
	case TextAlignment::BottomLeft:
		py -= nHeight;
		break;
	case TextAlignment::BottomCenter:
		px -= this->StrWidthPixels(str) / 2;
		py -= nHeight;
		break;
	case TextAlignment::BottomRight:
		px -= this->StrWidthPixels(str);
		py -= nHeight;
		break;
	default: // Default to top left alignment
		break;
	}
	this->DrawString(str, px, py, buffer, alphaColor, invert);
	nCursorY += nHeight;
}

uint16_t ott::CharacterMap::StrWidthPixels(const std::string& str) {

	return static_cast<uint16_t>(str.length()) * nWidth;
}

/*
 * Public functions
 */

void ott::getSmoothSpectrum(std::vector<ColorRGB>& palette, const uint16_t& nColors, const ColorRGB& startColor, const ColorRGB& endColor) {
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
	for (uint16_t i = 0; i < nColors; i++) {
		palette.push_back(ColorRGB(fColor[0], fColor[1], fColor[2]));
		for (uint32_t j = 0; j < 3; j++)
			fColor[j] += fDelta[j];
	}
}
