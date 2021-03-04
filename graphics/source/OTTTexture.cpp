#include <iostream>
#include <fstream>
#include <vector>

#include <SOIL/SOIL.h>
#include <GL/freeglut.h>

#include "OTTTexture.hpp"

OTTTexture::~OTTTexture() {
	unsigned int arr[1] = { nContext }; // Simple workaround (temporary)
	glDeleteTextures(1, arr);
}

unsigned int OTTTexture::getTexture() {
	if (nContext || !nBytes)
		return nContext;

	// Generate an OpenGL texture
	glGenTextures(1, &nContext);
	glBindTexture(GL_TEXTURE_2D, nContext);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, dptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Return the OpenGL texture id
	return nContext;
}

bool OTTTexture::getPixel(const int& x, const int& y, OTTLogicalColor& color) {
	if (x >= nWidth || y >= nHeight)
		return false;
	color = OTTLogicalColor(&data.get()[(nWidth * y + x) * 4]);
	return true;
}

void OTTTexture::setOpacity(const float& opacity) {
	if (opacity < 0.f || opacity > 1.f) // Opacity out of range
		return;
	OTTLogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.setAlpha(pixel.getA() * opacity);
		}
	}
}

void OTTTexture::invertColors() {
	OTTLogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.invert();
		}
	}
}

void OTTTexture::toGrayscale() {
	OTTLogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.toGrayscale();
		}
	}
}

void OTTTexture::removeAlpha() {
	OTTLogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.setAlpha(1.f);
		}
	}
}

void OTTTexture::colorToAlpha(const ColorRGB& chroma, const float& margin/*=0*/, const float& opacity/*=0*/) {
	OTTLogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			if (pixel.compareColor(chroma, margin))
				pixel.setAlpha(opacity);
		}
	}
}

void OTTTexture::fillColor(const ColorRGB& color, const BlendMode& mode/*=BlendMode::NORMAL*/) {
	OTTLogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			switch (mode) {
			case BlendMode::NORMAL:
				pixel.setColor(color);
				break;
			case BlendMode::ADD:
				pixel += color;
				break;
			case BlendMode::SUB:
				pixel -= color;
				break;
			case BlendMode::MULT:
				pixel *= color;
				break;
			case BlendMode::DIVIDE:
				pixel /= color;
				break;
			case BlendMode::DIFF:
				pixel.difference(color);
				break;
			case BlendMode::DARKEN:
				pixel.darken(color);
				break;
			case BlendMode::LIGHTEN:
				pixel.lighten(color);
				break;
			case BlendMode::REPLACE:
				pixel.setColor(color);
				break;
			default:
				std::cout << " [warning] Unknown layer blending mode." << std::endl;
				return;
			}
		}
	}
}

void OTTTexture::processImage(imageManipulationFunc func) {
	OTTLogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			(*func)(&pixel, j, i);
		}
	}
}

bool OTTTexture::read(const std::string& fname) {
	int W, H, ch;
	data.reset(SOIL_load_image(fname.c_str(), &W, &H, &ch, SOIL_LOAD_AUTO));
	nWidth = W;
	nHeight = H;
	nChannels = ch;
	nBytes = nWidth * nHeight * 4;
	dptr = data.get();
	if (nChannels != 4) { // Up-convert to RGBA
		bitmap.resize(nBytes); // Resize data vector for storing up-converted image data
		switch (nChannels) {
		case 1: // R (grayscale)
			for (int i = 0; i < nWidth * nHeight; i++) { // Over all pixels
				bitmap[4 * i + 0] = dptr[i];
				bitmap[4 * i + 1] = dptr[i];
				bitmap[4 * i + 2] = dptr[i];
				bitmap[4 * i + 3] = 255;
			}
			break;
		case 2: // RG (rare)
			for (int i = 0; i < nWidth * nHeight; i++) { // Over all pixels
				bitmap[4 * i + 0] = dptr[2 * i];
				bitmap[4 * i + 1] = dptr[2 * i + 1];
				bitmap[4 * i + 2] = 0;
				bitmap[4 * i + 3] = 255;
			}
			break;
		case 3: // RGB
			for (int i = 0; i < nWidth * nHeight; i++) { // Over all pixels
				bitmap[4 * i + 0] = dptr[3 * i];
				bitmap[4 * i + 1] = dptr[3 * i + 1];
				bitmap[4 * i + 2] = dptr[3 * i + 2];
				bitmap[4 * i + 3] = 255;
			}
			break;
		default:
			std::cout << " [error] Input image has invalid number of channels (" << nChannels << ")" << std::endl;
			return false;
		}
		// Image data now stored in data vector, free memory pointed to by unique_ptr
		data.reset(0x0);
		dptr = &bitmap[0];
	}
	std::cout << " [debug] Loaded image, W=" << nWidth << ", H=" << nHeight << ", with " << nChannels << " channels (" << nBytes/1E3 << " kB)" << std::endl;
	return (bGood = (nBytes > 0));
}

void OTTTexture::fill(const unsigned char& value/*=0*/) {
	std::fill(dptr, dptr + nBytes, value);
}

void OTTTexture::free() {
	// Free image memory
	data.reset(0x0);
	dptr = 0x0;
	bGood = false;
}
