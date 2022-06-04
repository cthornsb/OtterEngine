#ifndef GRAPHICS_OTT_TEXTURE_HPP
#define GRAPHICS_OTT_TEXTURE_HPP

#include <graphics/core/OTTColor.hpp>
#include <graphics/core/OTTImageBuffer.hpp>

#include <memory>
#include <string>

namespace ott {

class Texture : public ImageBuffer {
public:
	/** Default constructor
	  */
	Texture() :
		ImageBuffer(),
		nContext(0),
		sName(),
		data()
	{
	}

	/** Filename constructor
	  * @param fname Path to input image file (png, bmp, etc)
	  * @param name The name of the texture
	  */
	Texture(const std::string& fname, const std::string& name = "") :
		ImageBuffer(),
		nContext(0),
		sName(name),
		data()
	{
		this->Read(fname);
	}

	/** Blank image constructor
	  * @param W Width of new image (in pixels)
	  * @param H Height of new image (in pixels)
	  * @param name The name of the texture
	  * @param color The color to fill the blank image with
	  */
	Texture(const int32_t& W, const int32_t& H, const std::string& name = "", const ColorRGB& color = colors::White) :
		ImageBuffer(W, H, 4), // Blank RGBA image
		nContext(0),
		sName(name),
		data()
	{
		this->FillColor(color);
	}

	/** Copy constructor
	  */
	Texture(const Texture&) = delete;

	/** Destructor
	  */
	~Texture();

	/** Assignment operator
	  */
	Texture& operator = (const Texture&) = delete;

	/** Equality operator
	  */
	bool operator == (const std::string& name) const {
		return (name == sName);
	}

	/** Equality operator
	  */
	bool operator == (const uint32_t& id) const {
		return (id == nContext);
	}

	/** Get the name of the texture
	  */
	std::string Name() const {
		return sName;
	}

	/** Get the current OpenGL texture context
	  */
	uint32_t Context() const {
		return nContext;
	}

	/** Set the associated name of the texture
	  */
	void SetName(const std::string& name) {
		sName = name;
	}

	/** Send loaded image to OpenGL to create a texture. Once OpenGL creates the texture,
	  * it may no longer be modified directly. If texture modification is required, a new
	  * OpenGL texture must be created.
	  * @param bLinearFiltering If set, texture minimization and magnification are set to OpenGL linear filtering mode
	  */
	uint32_t GetTexture(bool bLinearFiltering = true);

	/** Set an OpenGL texture parameter (not implemented)
	  */
	void SetParameter() { }

	/** Generate OpenGL mipmaps (not implemented)
	  */
	void GenerateMipmaps() { }

	/**
	  */
	bool IncreaseColorDepth(const uint16_t& ch);

	/** Load an image into memory
	  * @note Image is loaded into conventional memory, getTexture() must be used to convert to OpenGL texture
	  * @param fname Path to image file
	  * @return True if image is loaded successfully and return false upon failure
	  */
	bool Read(const std::string& fname);

	/** Save image buffer to disk. If image format is RGBA, output file will be TGA, otherwise it will be BMP.
	  * @param fname Path to output image file (excluding extension, which will be added automatically)
	  * @return True if image is saved successfully
	  */
	bool Write(const std::string& fname);

	/** Fill image buffer with a value
	  * Uses std::fill to fill entire buffer with specified color component value.
	  */
	void Fill(const uint8_t& value = 0) override;

	/** Free conventional memory being used to store image
	  * @note Does not delete associated OpenGL texture
	  */
	void Free() override;

	/** Generate a new OpenGL texture with RGB color components and return its ID
	  */
	static uint32_t GenerateTextureRGB(const int32_t& W, const int32_t& H, const uint8_t* data, bool bLinearFilter = true);

	/** Generate a new OpenGL texture with RGBA color components and return its ID
	  */
	static uint32_t GenerateTextureRGBA(const int32_t& W, const int32_t& H, const uint8_t* data, bool bLinearFilter = true);

	/** Save image buffer to disk. If image format is RGBA, output file will be TGA, otherwise it will be BMP.
	  * The image buffer class saves images upside down because that is what OpenGL expects when pushing pixel
	  * data to the frame buffer. In order to save the image buffer as an image, we need to flip the image data
	  * vertically.
	  * @param buffer Image buffer bitmap to write to disk
	  * @param fname Path to output image file (excluding extension, which will be added automatically)
	  * @param bFlipX Flip image data horizontally in output file
	  * @param bFlipY Flip image data vertically in output file
	  * @return True if image is saved successfully
	  */
	static bool Write(const ImageBuffer* const buffer, const std::string& fname, bool bFlipX = false, bool bFlipY = false);

	/** Save image data to disk. If image format is RGBA, output file will be TGA, otherwise it will be BMP.
	  * The image buffer class saves images upside down because that is what OpenGL expects when pushing pixel
	  * data to the frame buffer. In order to save the image buffer as an image, we need to flip the image data
	  * vertically.
	  * @param buffer Image buffer bitmap to write to disk
	  * @param fname Path to output image file (excluding extension, which will be added automatically)
	  * @param W Width of input image data in pixels
	  * @param H Height of input image data in pixels
	  * @param ch Number of color channels per pixel (e.g. 3 for RGB or 4 for RGBA)
	  * @return True if image is saved successfully
	  */
	static bool Write(const uint8_t* const buffer, const std::string& fname, const int& W, const int& H, const int& ch);

protected:
	uint32_t nContext; ///< OpenGL texture context ID number

	std::string sName; ///< Name of texture

	std::unique_ptr<uint8_t> data; ///< Array containing image data
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_TEXTURE_HPP
