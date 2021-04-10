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
		nChannels = 4; // Image now in RGBA format
	}
	std::cout << " [debug] Loaded image, W=" << nWidth << ", H=" << nHeight << ", with " << nChannels << " channels (" << nBytes/1E3 << " kB)" << std::endl;
	return (bGood = (nBytes > 0));
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
}
