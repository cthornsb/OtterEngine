#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memcpy

#include <SOIL/SOIL.h>
#include <GL/freeglut.h>

#include "Texture.hpp"

Texture::~Texture() {
	free();
	unsigned int arr[1] = { nContext }; // Simple workaround (temporary)
	glDeleteTextures(1, arr);
}

unsigned int Texture::getTexture() {
	if (nContext || !nBytes)
		return nContext;

	// Generate an OpenGL texture
	glGenTextures(1, &nContext);
	glBindTexture(GL_TEXTURE_2D, nContext);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, getConstData());
	glBindTexture(GL_TEXTURE_2D, 0);

	// Return the OpenGL texture id
	return nContext;
}

bool Texture::getPixel(const int& x, const int& y, LogicalColor& color) {
	if (x >= nWidth || y >= nHeight)
		return false;
	color = LogicalColor(&data.get()[(nWidth * y + x) * 4]);
	return true;
}

void Texture::setOpacity(const float& opacity) {
	if (opacity < 0.f || opacity > 1.f) // Opacity out of range
		return;
	LogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.setAlpha(pixel.getA() * opacity);
		}
	}
}

void Texture::invertColors() {
	LogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.invert();
		}
	}
}

void Texture::toGrayscale() {
	LogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.toGrayscale();
		}
	}
}

void Texture::removeAlpha() {
	LogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.setAlpha(1.f);
		}
	}
}

void Texture::colorToAlpha(const ColorRGB& chroma, const float& margin/*=0*/, const float& opacity/*=0*/) {
	LogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			if (pixel.compareColor(chroma, margin))
				pixel.setAlpha(opacity);
		}
	}
}

void Texture::fillColor(const ColorRGB& color, const BlendMode& mode/*=BlendMode::NORMAL*/) {
	LogicalColor pixel;
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

void Texture::processImage(imageManipulationFunc func) {
	LogicalColor pixel;
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			(*func)(&pixel, j, i);
		}
	}
}

bool Texture::read(const std::string& fname) {
	data.reset(SOIL_load_image(fname.c_str(), &nWidth, &nHeight, &nChannels, SOIL_LOAD_AUTO));
	nBytes = nWidth * nHeight * 4;
	dptr = data.get();
	if (nChannels != 4) { // Up-convert to RGBA
		std::vector<unsigned char> newData(nBytes, 0);
		switch (nChannels) {
		case 1: // R (grayscale)
			for (int i = 0; i < nWidth * nHeight; i++) { // Over all pixels
				newData[4 * i + 0] = dptr[i];
				newData[4 * i + 1] = dptr[i];
				newData[4 * i + 2] = dptr[i];
				newData[4 * i + 3] = 255;
			}
			break;
		case 2: // RG (rare)
			for (int i = 0; i < nWidth * nHeight; i++) { // Over all pixels
				newData[4 * i + 0] = dptr[2 * i];
				newData[4 * i + 1] = dptr[2 * i + 1];
				newData[4 * i + 2] = 0;
				newData[4 * i + 3] = 255;
			}
			break;
		case 3: // RGB
			for (int i = 0; i < nWidth * nHeight; i++) { // Over all pixels
				newData[4 * i + 0] = dptr[3 * i];
				newData[4 * i + 1] = dptr[3 * i + 1];
				newData[4 * i + 2] = dptr[3 * i + 2];
				newData[4 * i + 3] = 255;
			}
			break;
		default:
			std::cout << " [error] Input image has invalid number of channels (" << nChannels << ")" << std::endl;
			return false;
		}
		// Replace image data
		data.reset(new unsigned char[nBytes]);
		memcpy(data.get(), newData.data(), nBytes);
		dptr = data.get();
	}
	std::cout << " [debug] Loaded image, W=" << nWidth << ", H=" << nHeight << ", with " << nChannels << " channels (" << nBytes/1E3 << " kB)" << std::endl;
	return (bGood = (nBytes > 0));
}

void Texture::free() {
	// Free image memory
	data.reset(0x0);
	dptr = 0x0;
	bGood = false;
}
