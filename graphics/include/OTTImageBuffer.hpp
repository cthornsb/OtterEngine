#ifndef OTT_IMAGE_BUFFER_HPP
#define OTT_IMAGE_BUFFER_HPP

#include <vector>

#include "ColorRGB.hpp"
#include "OTTLogicalColor.hpp"

typedef void (*imageManipulationFunc)(OTTLogicalColor*, const int&, const int&);

/** Layer blending modes
  **/
enum class BlendMode {
	NONE,    ///< Do not modify existing pixel
	NORMAL,  ///< The bottom layer is completely overwritten by the top layer, alpha values are ignored
	ADD,     ///< Components of top layer are added to components of bottom layer
	SUB,     ///< Components of top layer are subtracted from components of bottom layer
	DIFF,    ///< Difference is computed between top and bottom colors, ensuring a positive result
	MULT,    ///< Components of bottom color are multiplied by components of top color
	DIVIDE,  ///< Components of bottom color are divided by components of top color
	DARKEN,  ///< The minimum value is taken for each of the color components of the top and bottom layers
	LIGHTEN, ///< The maximum value is taken for each of the color components of the top and bottom layers
	AVERAGE, ///< Average the bottom and top layer colors
	REPLACE  ///< Functionally the same as NORMAL
};

enum class DrawMode {
	NONE,    ///< Do not modify existing pixels
	OUTLINE, ///< Draw only the outline of shapes
	FILL     ///< Fill all pixels inside of drawn shapes
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
		dptr(0x0),
		colorBlendMode(BlendMode::NORMAL),
		currentDrawColor(Colors::WHITE)
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
		dptr(&bitmap[0]),
		colorBlendMode(BlendMode::NORMAL),
		currentDrawColor(Colors::WHITE)
	{
	}
	
	/** Copy constructor
	  */
	OTTImageBuffer(const OTTImageBuffer&) = delete;

	/** Assignment operator
	  */
	OTTImageBuffer& operator = (const OTTImageBuffer&) = delete;
	
	/** Destructor
	  */ 
	~OTTImageBuffer();

	/** Return true if image data is loaded in conventional memory and return false otherwise
	  **/
	bool isGood() const { 
		return bGood; 
	}

	/** Get a const pointer to color data for a specified pixel
	  */
	const unsigned char* operator [] (const unsigned int& pixel) const {
		return const_cast<unsigned char*>(&dptr[pixel * nChannels]);
	}

	/** Get a pointer to color data for a specified pixel
	  */
	unsigned char* operator [] (const unsigned int& pixel) {
		return &dptr[pixel * nChannels];
	}

	/** Get a const pointer to color data for a specified pixel
	  */
	const unsigned char* get(const unsigned short& x, const unsigned short& y) const {
		return const_cast<unsigned char*>(&dptr[(nWidth * y + x) * nChannels]);
	}

	/** Get a pointer to color data for a specified pixel
	  */
	unsigned char* get(const unsigned short& x, const unsigned short& y) {
		return &dptr[(nWidth * y + x) * nChannels];
	}

	/** Get a const pointer to start of image data
	  */
	const unsigned char* get() const {
		return const_cast<unsigned char*>(&dptr[0]);
	}

	/** Get a pointer to start of image data
	  */
	unsigned char* get() {
		return &dptr[0];
	}

	/** Get a logical pixel from a pixel in the loaded image
	  * @return True if the specified pixel is a valid location
	  **/
	bool getPixel(const int& x, const int& y, OTTLogicalColor& color);

	/** Get a pointer to the start of a pixel color data
	  * @return A pointer to the start of the pixel's color data, or return NULL if the location is invalid
	  **/
	const unsigned char* getPixel(const int& x, const int& y) const ;

	/** Copy image buffer pixel data to a vector.
	  * Vector will be cleared before filling with pixel data.
	  */	
	void copyImageData(std::vector<unsigned char>& output) const ;
	
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

	/** Get the number of pixel color channels
	  */
	unsigned short getNumChannels() const {
		return nChannels;
	}
	
	/** Get the total number of image pixels
	  */
	unsigned int getNumPixels() const {
		return nSize;
	}
	
	/** Get the total number of bytes of image data
	  */
	unsigned int getNumBytes() const {
		return nBytes;
	}

	/** Return true if buffer data is empty
	  */
	bool empty() const {
		return (nSize == 0);
	}

	/** Resize image buffer
	  */
	void resize(const unsigned short& W, const unsigned short& H, const unsigned short& ch);

	/** Set the current pixel color blending mode. The default blending mode is 
	  * BlendMode::NORMAL, which simply replaces the existing color with a new one.
	  */
	void setBlendMode(const BlendMode& mode = BlendMode::NORMAL) {
		colorBlendMode = mode;
	}

	/** Set the draw color
	  */
	void setDrawColor(const ColorRGB& color) {
		currentDrawColor = color;
	}
	
	/** Set the color of a pixel in the buffer, ignoring the current color blending mode
	  */
	void setPixel(const unsigned short& x, const unsigned short& y, const ColorRGB& color);

	/** Set the color of a pixel in the buffer, ignoring the current color blending mode
	  */
	void setPixel(const unsigned short& x, const unsigned short& y, const OTTLogicalColor& color);

	/** Set the color of a horizontal row of pixels, ignoring the current color blending mode
	  */
	void setPixelRow(const unsigned short& y, const ColorRGB& color);

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
	  **/
	void fillColor(const ColorRGB& color);

	/** Draw an external image buffer into a region of this buffer with upper left corner at (x,y)
	  */
	void blendSubImage(const unsigned short& x, const unsigned short& y, OTTImageBuffer* buffer, const float& alpha);

	/** Draw an external image buffer into a region of this buffer with upper left corner at (x,y)
	  */
	void blendSubImage(const unsigned short& x, const unsigned short& y, OTTImageBuffer* buffer);

	/** Copy an external image buffer into a region of this buffer with upper left corner at (x,y)
	  * This method expects the external image buffer to be smaller than this buffer. All pixels from
	  * the external buffer will be copied into this one starting at (x,y).
	  * See also drawPixels.
	  */
	void drawSubImage(const unsigned short& x, const unsigned short& y, OTTImageBuffer* buffer);

	/** Draw a pixel at the position (x,y)
	  */
	void drawPixel(const unsigned short& x, const unsigned short& y);

	/** Draw a series of (x,y) pairs
	  */
	void drawPixels(const std::vector<std::pair<unsigned short, unsigned short> >& vertices);

	/** Draw a series of N (x,y) pairs.
	  * Input vertex array should be an ordered list of X and Y pairs and must be at least 2 * N elements long.
	  */
	void drawPixels(const unsigned short* vertices, const size_t& N);

	/** Copy a region of an external image buffer with upper left corner at (x,y) into this one.
	  * This method expects the external image buffer to be larger than this buffer. A region of pixels
	  * with the width and height of this buffer will be extracted from the larger external buffer. 
	  * See also drawSubImage.
	  */
	void drawPixels(const unsigned short& x, const unsigned short& y, OTTImageBuffer* buffer);

	/** Draw a line between the two points (x0,y0) and (x1,y1)
	  */
	void drawLine(
		const unsigned short& x0, const unsigned short& y0, 
		const unsigned short& x1, const unsigned short& y1
	);

	/** Draw a sequence of lines between a series of (x,y) pairs
	  */
	void drawLines(const std::vector<std::pair<unsigned short, unsigned short> >& vertices);

	/** Draw a sequence of lines between a series of N (x,y) pairs.
	  * Input vertex array should be an ordered list of X and Y pairs and must be at least 2 * N elements long.
	  */
	void drawLines(const unsigned short* vertices, const size_t& N);

	/** Draw a closed polygon with N sides using a series of N (x,y) pairs
	  */
	void drawPolygon(const std::vector<std::pair<unsigned short, unsigned short> >& vertices);

	/** Draw a closed polygon with N sides.
	  * Input vertex array should be an ordered list of X and Y pairs and must be at least 2 * N elements long.
	  */
	void drawPolygon(const unsigned short* vertices, const size_t& N);

	/** Draw a rectangle with top left corner (x0,y0) and bottom right corner (x1,y1)
	  */
	void drawRectangle(
		const unsigned short& x0, const unsigned short& y0,
		const unsigned short& x1, const unsigned short& y1,
		bool bFilled = false
	);

	/** Draw a circle with center (x0,y0) and specified pixel radius
	  */
	void drawCircle(const float& x0, const float& y0, const float& radius, bool bFilled = false);

	/** Draw an ellipse with a specified center (x0,y0), semi-major radius (A), and semi-minor radius (B)
	  */
	void drawEllipse(const unsigned short& x0, const unsigned short& y0, const float& A, const float& B, bool bFilled = false);

	/** Draw a regular N-sided polygon centered at (x0,y0) and whose vertices are circumscribed by a circle with the specified radius
	  */
	void drawRegularPolygon(const unsigned short& x0, const unsigned short& y0,	const unsigned short& N, const float& radius);

	/** Draw an arbitrary triangle with three specified vertices (x0,y0), (x1,y1), and (x2,y2)
	  */
	void drawTriangle(
		const unsigned short& x0, const unsigned short& y0,
		const unsigned short& x1, const unsigned short& y1,
		const unsigned short& x2, const unsigned short& y2
	);

	/** Draw an arbitrary quad with four specified vertices (x0,y0), (x1,y1), (x2,y2), and (x3,y3)
	  */
	void drawQuad(
		const unsigned short& x0, const unsigned short& y0,
		const unsigned short& x1, const unsigned short& y1,
		const unsigned short& x2, const unsigned short& y2,
		const unsigned short& x3, const unsigned short& y3
	);

	/** Manipulate the loaded image on the pixel level
	  * @param func Function pointer which will be called for every pixel in the image
	  **/
	void processImage(imageManipulationFunc func);

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

	/** Get a vector of pointers of pairs representing the destination and source of pixel data for all rows inside
	  * a bounding box with upper left corner at pixel location (x,y) in the destination image, at (0,0) in the
	  * source image, and with width and height taken from the source image.
	  * This allows for fast image data copying, assuming the color depth is the same for both images.
	  * @return The number of rows which may be safely copied, given a width of W pixels
	  */
	static size_t getImageTargets(
		const int& x, const int& y,
		OTTImageBuffer* dest,
		OTTImageBuffer* src,
		std::vector<std::pair<unsigned char*, unsigned char*> >& targets
	);

	/** Get a vector of pointers of pairs representing the destination and source of pixel data for all rows inside
	  * a bounding box with upper left corner at pixel location (x0,y0) in the destination image, at (x1,y1) in the 
	  * source image, and with width W and height H (both in pixels).
	  * This allows for fast image data copying, assuming the color depth is the same for both images.
	  * @return The number of rows which may be safely copied, given a width of W pixels
	  */
	static size_t getImageTargets(
		const int& x0, const int& y0,
		const int& x1, const int& y1,
		const int& W, const int& H,
		OTTImageBuffer* dest,
		OTTImageBuffer* src,
		std::vector<std::pair<unsigned char*, unsigned char*> >& targets
	);

protected:
	bool bGood; ///< Set if image buffer memory allocated successfully

	unsigned short nWidth; ///< Width of image buffer in pixels
	
	unsigned short nHeight; ///< Height of image buffer in pixels
	
	unsigned short nChannels; ///< Number of color channels
	
	unsigned int nSize; ///< Total size of image buffer in pixels
	
	unsigned int nBytes; ///< Total size of image in bytes
	
	std::vector<unsigned char> bitmap; ///< Image data
	
	unsigned char* dptr; ///< Internal pointer to image data

	BlendMode colorBlendMode; ///< The current pixel blend mode

	ColorRGB currentDrawColor; ///< Current drawing color for pixel color blending operations

	/** Blend the current draw color into the image
	  */
	void blendPixel(const unsigned short& px, const unsigned short& py);

	/** Blend a single pixel color into the image
	  */
	void blendPixel(const unsigned short& px, const unsigned short& py, const ColorRGB& color);

	/** Blend a single pixel color into the image
	  */
	void blendPixel(OTTLogicalColor& dest, const ColorRGB& color) const;

	/** Blend a single logical pixel color into the image
	  */
	void blendPixel(OTTLogicalColor& dest, const OTTLogicalColor& src) const;
};

#endif
