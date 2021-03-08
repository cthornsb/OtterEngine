#ifndef OTT_IMAGE_BUFFER_HPP
#define OTT_IMAGE_BUFFER_HPP

#include <vector>

#include "ColorRGB.hpp"
#include "OTTLogicalColor.hpp"

typedef void (*imageManipulationFunc)(OTTLogicalColor*, const int&, const int&);

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

class OTTImageBuffer{
public:
	/** Default constructor
	  */
	OTTImageBuffer() :
		bGood(false),
		nWidth(0),
		nHeight(0),
		nChannels(3),
		nSize(0),
		nBytes(0),
		bitmap(),
		dptr(0x0)
	{
	}
	
	/** Width and height constructor
	  */
	OTTImageBuffer(const unsigned short& W, const unsigned short& H, const unsigned short& ch=3) :
		bGood(true),
		nWidth(W),
		nHeight(H),
		nChannels(ch),
		nSize(W * H),
		nBytes(nSize * ch), // ch Color channels
		bitmap(nBytes, 0),
		dptr(&bitmap[0])
	{
	}
	
	/** Copy constructor
	  * Copy image size parameters but do not copy data.
	  */
	OTTImageBuffer(const OTTImageBuffer& other) : 
		bGood(other.bGood),
		nWidth(other.nWidth),
		nHeight(other.nHeight),
		nChannels(other.nChannels),
		nSize(other.nSize),
		nBytes(other.nBytes), // ch Color channels
		bitmap(),
		dptr(other.dptr)
	{
	}
	
	/** Assignment operator
	  */
	OTTImageBuffer& operator = (const OTTImageBuffer&) = delete;
	
	/** Destructor
	  */ 
	~OTTImageBuffer(){ }

	/** Return true if image data is loaded in conventional memory and return false otherwise
	  **/
	bool isGood() const { 
		return bGood; 
	}

	/** Get a const pointer to color data for a specified pixel
	  */
	const unsigned char* operator [] (const unsigned int& pixel) const {
		return static_cast<const unsigned char*>(&dptr[pixel * nChannels]);
	}

	/** Get a pointer to color data for a specified pixel
	  */
	unsigned char* operator [] (const unsigned int& pixel) {
		return &dptr[pixel * nChannels];
	}

	/** Get a const pointer to color data for a specified pixel
	  */
	const unsigned char* get(const unsigned short& x, const unsigned short& y) const {
		return static_cast<const unsigned char*>(&dptr[(nWidth * (nHeight - y - 1) + x) * nChannels]);
	}

	/** Get a pointer to color data for a specified pixel
	  */
	unsigned char* get(const unsigned short& x, const unsigned short& y) {
		return &dptr[(nWidth * (nHeight - y - 1) + x) * nChannels];
	}

	/** Get a const pointer to start of image data
	  */
	const unsigned char* get() const {
		return static_cast<const unsigned char*>(&dptr[0]);
	}

	/** Get a pointer to start of image data
	  */
	unsigned char* get() {
		return &dptr[0];
	}

	/** Get the true width of the loaded image (in pixels)
	  **/
	unsigned short getWidth() const {
		return nWidth; 
	}

	/** Get the true height of the loaded image (in pixels)
	  **/
	unsigned short getHeight() const { 
		return nHeight; 
	}

	/** Return true if buffer data is empty
	  */
	bool empty() const {
		return (nSize > 0);
	}

	/** Resize image buffer
	  */
	void resize(const unsigned short& W, const unsigned short& H);
	
	/** Set the color of a pixel in the buffer
	  */
	void setPixel(const unsigned short& x, const unsigned short& y, const ColorRGB& color);

	/** Set the color of a horizontal row of pixels
	  */
	void setPixelRow(const unsigned short& y, const ColorRGB& color);

	/** Clear image buffer to a specified color
	  * For monochromatic colors, it should be faster to use fill().
	  */
	void clear(const ColorRGB& color);

	/** Fill image buffer with a value
	  * Uses std::fill to fill entire buffer with specified color component value.
	  */
	virtual void fill(const unsigned char& value=0);

	/** Free conventional memory being used to store image
	  */
	virtual void free();

protected:
	bool bGood; ///< Set if image buffer memory allocated successfully

	unsigned short nWidth; ///< Width of image buffer in pixels
	
	unsigned short nHeight; ///< Height of image buffer in pixels
	
	unsigned short nChannels; ///< Number of color channels
	
	unsigned int nSize; ///< Total size of image buffer in pixels
	
	unsigned int nBytes; ///< Total size of image in bytes
	
	std::vector<unsigned char> bitmap; ///< Image data
	
	unsigned char* dptr; ///< Internal pointer to image data
};

#endif
