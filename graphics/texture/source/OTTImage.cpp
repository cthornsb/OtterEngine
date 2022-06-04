#include "OTTImage.hpp"
#include "OTTTexture.hpp"

#include <graphics/core/OTTImageBuffer.hpp>

#include <GLFW/glfw3.h>

bool ott::SaveImageBufferToBitmap(const ImageBuffer* const buffer, const std::string& fname) {
	return Texture::Write(buffer, fname, false, true);
}

bool ott::SaveFrameBufferToBitmap(const std::string& fname, const int& width, const int& height) {
	uint8_t* data = nullptr;
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	Texture::Write(data, fname, width, height, 3);
	delete[] data;
	return false;
}
