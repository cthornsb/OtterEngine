#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <memory>
#include <string>

#include "ColorRGB.hpp"
#include "LogicalColor.hpp"

typedef void (*imageManipulationFunc)(LogicalColor*, const int&, const int&);

/** Layer blending modes
  **/
enum class BlendMode {
	NORMAL,  ///< The bottom layer is completely overwritten by the top layer, alpha values are ignored
	ADD,     ///< Components of top layer are added to components of bottom layer
	SUB,     ///< Components of top layer are subtracted from components of bottom layer
	DIFF,    ///< Difference is computed between top and bottom colors, ensuring a positive result
	MULT,    ///< Components of bottom color are multiplied by components of top color
	DIVIDE,  ///< Components of bottom color are divided by components of top color
	DARKEN,  ///< The minimum value is taken for each of the color components of the top and bottom layers
	LIGHTEN, ///< The maximum value is taken for each of the color components of the top and bottom layers
	REPLACE  ///< Functionally the same as NORMAL
};

class Texture {
public:
	/** Default constructor
	  **/
	Texture() :
		nWidth(0),
		nHeight(0),
		nChannels(0),
		nBytes(0),
		nContext(0),
		bGood(false),
		sName(),
		data(),
		dptr(0x0)
	{
	}

	/** Copy constructor
	  * @note The pointer to image data is set, but no data is copied
	  **/
	Texture(const Texture& tex) :
		nWidth(tex.nWidth),
		nHeight(tex.nHeight),
		nChannels(tex.nChannels),
		nBytes(tex.nBytes),
		nContext(tex.nContext),
		bGood(tex.bGood),
		sName(tex.sName),
		data(),
		dptr(tex.dptr)
	{
	}

	/** Filename constructor
	  * @param fname Path to input image file (png, bmp, etc)
	  * @param name The name of the texture
	  **/
	Texture(const std::string& fname, const std::string& name="") :
		nWidth(0),
		nHeight(0),
		nChannels(0),
		nBytes(0),
		nContext(0),
		bGood(false),
		sName(name),
		data(),
		dptr(0x0)
	{
		read(fname);
	}

	/** Blank image constructor
	  * @param W Width of new image (in pixels)
	  * @param H Height of new image (in pixels)
	  * @param name The name of the texture
	  * @param color The color to fill the blank image with
	  **/
	Texture(const int& W, const int& H, const std::string& name = "", const ColorRGB& color=Colors::WHITE) :
		nWidth(W),
		nHeight(H),
		nChannels(4),
		nBytes(W * H * 4),
		nContext(0),
		bGood(true),
		sName(name),
		data(new unsigned char[nBytes]),
		dptr(data.get())
	{
		fillColor(color, BlendMode::NORMAL);
	}

	/** Destructor
	  **/
	~Texture() {
		free();
	}

	/** Return true if image data is loaded in conventional memory and return false otherwise
	  **/
	bool isGood() const { return bGood; }

	/** Get the true width of the loaded image (in pixels)
	  **/
	int getWidth() const { return nWidth; }

	/** Get the true height of the loaded image (in pixels)
	  **/
	int getHeight() const { return nHeight; }

	/** Get the current OpenGL texture context
	  **/
	unsigned int getContext() const { return nContext; }

	/** Send loaded image to OpenGL to create a texture. Once OpenGL creates the texture,
	  * it may no longer be modified directly. If texture modification is required, a new
	  * OpenGL texture must be created. 
	  **/
	unsigned int getTexture();

	/** Get a const pointer to raw image data in conventional memory
	  **/
	const unsigned char* getConstData() const { return dptr; }

	/** Get a logical pixel from a pixel in the loaded image
	  **/
	bool getPixel(const int& x, const int& y, LogicalColor& color);

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

	/** Free conventional memory being used to store image
	  * @note Does not delete associated OpenGL texture
	  **/
	void free();

protected:
	int nWidth;
	int nHeight;
	int nChannels;
	int nBytes;

	unsigned int nContext;

	bool bGood;

	std::string sName;

	std::unique_ptr<unsigned char> data;

	const unsigned char* dptr;
};

#endif