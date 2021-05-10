#ifndef OTT_TEXTURE_HPP
#define OTT_TEXTURE_HPP

#include <memory>
#include <string>

#include "ColorRGB.hpp"
#include "OTTImageBuffer.hpp"

class OTTTexture : public OTTImageBuffer {
public:
	/** Default constructor
	  **/
	OTTTexture() :
		OTTImageBuffer(),
		nContext(0),
		sName(),
		data()
	{
	}

	/** Filename constructor
	  * @param fname Path to input image file (png, bmp, etc)
	  * @param name The name of the texture
	  **/
	OTTTexture(const std::string& fname, const std::string& name="") :
		OTTImageBuffer(),
		nContext(0),
		sName(name),
		data()
	{
		read(fname);
	}

	/** Blank image constructor
	  * @param W Width of new image (in pixels)
	  * @param H Height of new image (in pixels)
	  * @param name The name of the texture
	  * @param color The color to fill the blank image with
	  **/
	OTTTexture(const int& W, const int& H, const std::string& name = "", const ColorRGB& color=Colors::WHITE) :
		OTTImageBuffer(W, H, 4), // Blank RGBA image
		nContext(0),
		sName(name),
		data()
	{
		fillColor(color);
	}

	/** Copy constructor
	  */
	OTTTexture(const OTTTexture&) = delete;

	/** Assignment operator
	  */
	OTTTexture& operator = (const OTTTexture&) = delete;

	/** Destructor
	  **/
	~OTTTexture();

	/** Equality operator
	  */
	bool operator == (const std::string& name) const {
		return (name == sName);
	}

	/** Equality operator
	  */
	bool operator == (const unsigned int& id) const {
		return (id == nContext);
	}

	/** Get the name of the texture
	  */
	std::string getName() const {
		return sName;
	}

	/** Get the current OpenGL texture context
	  **/
	unsigned int getContext() const { 
		return nContext; 
	}

	/** Set the associated name of the texture
	  */
	void setName(const std::string& name) {
		sName = name;
	}

	/** Send loaded image to OpenGL to create a texture. Once OpenGL creates the texture,
	  * it may no longer be modified directly. If texture modification is required, a new
	  * OpenGL texture must be created. 
	  * @param bLinearFiltering If set, texture minimization and magnification are set to OpenGL linear filtering mode
	  **/
	unsigned int getTexture(bool bLinearFiltering = true);

	/** Set an OpenGL texture parameter (not implemented)
	  */
	void setParameter() { }

	/** Generate OpenGL mipmaps (not implemented)
	  */
	void generateMipmaps() { }

	/**
	  */
	bool increaseColorDepth(const unsigned short& ch);

	/** Load an image into memory
	  * @note Image is loaded into conventional memory, getTexture() must be used to convert to OpenGL texture
	  * @param fname Path to image file
	  * @return True if image is loaded successfully and return false upon failure
	  **/
	bool read(const std::string& fname);

	/** Save image buffer to disk. If image format is RGBA, output file will be TGA, otherwise it will be BMP.
	  * @param fname Path to output image file (excluding extension, which will be added automatically)
	  * @return True if image is saved successfully
	  */
	bool write(const std::string& fname);

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
	static bool write(const OTTImageBuffer& buffer, const std::string& fname, bool bFlipX=false, bool bFlipY=false);

	/** Fill image buffer with a value
	  * Uses std::fill to fill entire buffer with specified color component value.
	  */
	void fill(const unsigned char& value=0) override;

	/** Free conventional memory being used to store image
	  * @note Does not delete associated OpenGL texture
	  **/
	void free() override;
	
	/** Generate a new OpenGL texture with RGB color components and return its ID
	  */
	static unsigned int generateTextureRGB(const int& W, const int& H, const unsigned char* data, bool bLinearFilter = true);
	
	/** Generate a new OpenGL texture with RGBA color components and return its ID
	  */
	static unsigned int generateTextureRGBA(const int& W, const int& H, const unsigned char* data, bool bLinearFilter = true);

protected:
	unsigned int nContext; ///< OpenGL texture context ID number

	std::string sName; ///< Name of texture

	std::unique_ptr<unsigned char> data; ///< Array containing image data
};

#endif
