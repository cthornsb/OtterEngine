#ifndef GRAPHICS_TEXTURE_OTT_IMAGE_HPP
#define GRAPHICS_TEXTURE_OTT_IMAGE_HPP

#include <string>

namespace ott {

class ImageBuffer;

/** Save current image buffer bitmap to disk
	* @param fname Path to output image file
	* @return True if image is saved successfully
	*/
bool SaveImageBufferToBitmap(const ImageBuffer* const buffer, const std::string& fname);

/** Save current OpenGL frame buffer to disk
	* @param fname Path to output image file
	* @param width Width of frame buffer in pixels
	* @param height Height of frame buffer in pixels
	* @return True if image is saved successfully
	*/
bool SaveFrameBufferToBitmap(const std::string& fname, const int& width, const int& height);

} /* namespace ott */

#endif // #ifndef GRAPHICS_TEXTURE_OTT_IMAGE_HPP
