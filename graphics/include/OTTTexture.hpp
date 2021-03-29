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

	/** Copy constructor
	  * @note The pointer to image data is set, but no data is copied
	  **/
	OTTTexture(const OTTTexture& tex) :
		OTTImageBuffer(tex),
		nContext(tex.nContext),
		sName(tex.sName),
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
		fillColor(color, BlendMode::NORMAL);
	}

	/** Destructor
	  **/
	~OTTTexture();

	/** Get the current OpenGL texture context
	  **/
	unsigned int getContext() const { 
		return nContext; 
	}

	/** Send loaded image to OpenGL to create a texture. Once OpenGL creates the texture,
	  * it may no longer be modified directly. If texture modification is required, a new
	  * OpenGL texture must be created. 
	  **/
	unsigned int getTexture();

	/** Set an OpenGL texture parameter (not implemented)
	  */
	void setParameter() { }

	/** Generate OpenGL mipmaps (not implemented)
	  */
	void generateMipmaps() { }

	/** Set multiplicitive opacity of the texture
	  * @param opacity Opacity of texture, in range [0, 1]
	  **/
	void setOpacity(const float& opacity);

	/** Invert image colors
	  **/
	void invertColors();

	/** Convert image to grayscale based on sRGB convention
	  **/
	void toGrayscale();

	/** Set all pixel alpha values to one (i.e. remove alpha channel). This may not be undone
	  **/
	void removeAlpha();

	/** Set a color in the image to translucent
	  * @param chroma Color to make translucent (chroma key)
	  * @param margin Margin of error for color match (0 is exact match, 1 accepts any color)
	  * @param opacity Opacity of translucent pixels (default is 0, i.e. fully transparent)
	  **/
	void colorToAlpha(const ColorRGB& chroma, const float& margin = 0, const float& opacity = 0);

	/** Fill the image with a color
	  * @param color Color to fill the image with
	  * @param mode Blending mode of color (default is normal blending)
	  **/
	void fillColor(const ColorRGB& color, const BlendMode& mode = BlendMode::NORMAL);

	/** Manipulate the loaded image on the pixel level
	  * @param func Function pointer which will be called for every pixel in the image
	  **/
	void processImage(imageManipulationFunc func);

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

protected:
	unsigned int nContext; ///< OpenGL texture context ID number

	std::string sName; ///< Name of texture

	std::unique_ptr<unsigned char> data; ///< Array containing image data
};

#endif
