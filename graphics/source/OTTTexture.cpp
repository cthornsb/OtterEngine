#include <iostream>
#include <fstream>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <SOIL/SOIL.h>

#include "OTTTexture.hpp"

OTTTexture::~OTTTexture() {
	free(); // Free image memory and delete OpenGL texture
}

unsigned int OTTTexture::getTexture(bool bLinearFiltering/* = true*/) {
	if (nContext || !nBytes)
		return nContext;

	if (nChannels < 4) { // Add alpha channel, if not available
		increaseColorDepth(4);
	}

	// Generate an OpenGL texture
	glGenTextures(1, &nContext);
	glBindTexture(GL_TEXTURE_2D, nContext);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (bLinearFiltering) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, dptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Return the OpenGL texture id
	return nContext;
}

bool OTTTexture::increaseColorDepth(const unsigned short& ch) {
	if (!bGood || ch <= nChannels || ch > 4) // Disallow downgrades and color channels above 4
		return false;
	nBytes = nWidth * nHeight * ch;
	bitmap.resize(nBytes); // Resize data vector for storing up-converted image data
	switch (nChannels) {
	case 1: // R (grayscale)
		for (unsigned int i = 0; i < nSize; i++) { // Over all pixels
			for (unsigned short j = 0; j < ch; j++) { // Over all color channels
				bitmap[4 * i + j] = (j != 3 ? dptr[i] : 255);
			}
		}
		break;
	case 2: // RG, RB, etc (rare)
		for (unsigned int i = 0; i < nSize; i++) { // Over all pixels
			bitmap[4 * i + 0] = dptr[2 * i];
			bitmap[4 * i + 1] = dptr[2 * i + 1];
			if(ch >= 3)
				bitmap[4 * i + 2] = 0;
			if(ch == 4)
				bitmap[4 * i + 3] = 255;
		}
		break;
	case 3: // RGB, BGR, etc
		for (unsigned int i = 0; i < nSize; i++) { // Over all pixels
			for (unsigned short j = 0; j < ch; j++) { // Over all color channels
				bitmap[4 * i + j] = (j != 3 ? dptr[3 * i + j] : 255);
			}
		}
		break;
	default:
		return false;
	}
	// Image data now stored in data vector, free memory pointed to by unique_ptr
	data.reset(0x0);
	dptr = &bitmap[0];
	nChannels = ch; // Image now in new format
	return true;
}

bool OTTTexture::read(const std::string& fname) {
	int W, H, ch;
	data.reset(SOIL_load_image(fname.c_str(), &W, &H, &ch, SOIL_LOAD_AUTO));
	nWidth = W;
	nHeight = H;
	nChannels = ch;
	nSize = nWidth * nHeight;
	nBytes = nSize * nChannels;
	dptr = data.get();
	bGood = (nBytes > 0);
	if(bGood)
		std::cout << " [debug] Loaded image, W=" << nWidth << ", H=" << nHeight << ", with " << nChannels << " channels (" << nBytes/1E3 << " kB)" << std::endl;
	else
		std::cout << " [error] Failed to load image." << std::endl;
	return bGood;
}

bool OTTTexture::write(const std::string& fname){
	// Note: The image buffer class saves images upside down because that is what
	// OpenGL expects when pushing pixel data to the frame buffer. In order to save
	// the image buffer as an image, we need to flip the image data vertically.
	return OTTTexture::write(*this, fname, false, true);
}

bool OTTTexture::write(const OTTImageBuffer& buffer, const std::string& fname, bool bFlipX, bool bFlipY){
	// Make a copy of image buffer data
	std::vector<unsigned char> copyOfData;
	buffer.copyImageData(copyOfData);
	
	// Reverse the pixels (without reversing the color components)
	unsigned short W = buffer.getWidth();
	unsigned short H = buffer.getHeight();
	unsigned short ch = buffer.getNumChannels();
	OTTLogicalColor outPixel;
	for (int i = 0; i < H; i++) { // Over rows
		for (int j = 0; j < W; j++) { // Over columns
			const unsigned char* inPixel = buffer.getPixel(
				(bFlipX ? (W - 1) - j : j), 
				(bFlipY ? (H - 1) - i : i)
			);
			outPixel = &copyOfData[(W * i + j) * ch];
			outPixel.setColor(inPixel);
		}
	}
	int retval = SOIL_save_image(
		(fname + ".bmp").c_str(), 
		SOIL_SAVE_TYPE_BMP, //(nChannels == 4 ? SOIL_SAVE_TYPE_TGA : SOIL_SAVE_TYPE_BMP), 
		W,
		H,
		ch,
		copyOfData.data()
	);
	return (retval == 1);
}

void OTTTexture::fill(const unsigned char& value/*=0*/) {
	std::fill(dptr, dptr + nBytes, value);
}

void OTTTexture::free() {
	// Free image memory
	data.reset(0x0);
	dptr = 0x0;
	bGood = false;
	if (nContext) { // Delete the OpenGL texture
		glDeleteTextures(1, &nContext);
		nContext = 0;
	}
}

unsigned int OTTTexture::generateTextureRGB(const int& W, const int& H, const unsigned char* data, bool bLinearFilter/* = true*/) {
	unsigned int retval = 0;
	glGenTextures(1, &retval);
	glBindTexture(GL_TEXTURE_2D, retval);
	if (bLinearFilter) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return retval;
}

unsigned int OTTTexture::generateTextureRGBA(const int& W, const int& H, const unsigned char* data, bool bLinearFilter/* = true*/) {
	unsigned int retval = 0;
	glGenTextures(1, &retval);
	glBindTexture(GL_TEXTURE_2D, retval);
	if (bLinearFilter) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	return retval;
}

