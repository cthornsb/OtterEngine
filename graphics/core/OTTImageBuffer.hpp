#ifndef GRAPHICS_OTT_IMAGE_BUFFER_HPP
#define GRAPHICS_OTT_IMAGE_BUFFER_HPP

#include "OTTColor.hpp"

#include <cstdint>
#include <vector>

namespace ott {

class LogicalColor;

typedef void (*ImageManipulationFunc_t)(LogicalColor*, const int32_t&, const int32_t&);

/** Layer blending modes
  */
enum class BlendMode {
	None,    ///< Do not modify existing pixel
	Normal,  ///< The bottom layer is completely overwritten by the top layer, alpha values are ignored
	Add,     ///< Components of top layer are added to components of bottom layer
	Sub,     ///< Components of top layer are subtracted from components of bottom layer
	Diff,    ///< Difference is computed between top and bottom colors, ensuring a positive result
	Mult,    ///< Components of bottom color are multiplied by components of top color
	Divide,  ///< Components of bottom color are divided by components of top color
	Darken,  ///< The minimum value is taken for each of the color components of the top and bottom layers
	Lighten, ///< The maximum value is taken for each of the color components of the top and bottom layers
	Average, ///< Average the bottom and top layer colors
	Replace  ///< Functionally the same as NORMAL
};

class ImageBuffer {
public:

	enum class DrawMode {
		None,    ///< Do not modify existing pixels
		Outline, ///< Draw only the outline of shapes
		Fill     ///< Fill all pixels inside of drawn shapes
	};

	/** Default constructor
	  */
	ImageBuffer() :
		bGood(false),
		nWidth(0),
		nHeight(0),
		nChannels(3),
		nSize(0),
		nBytes(0),
		bitmap(),
		dptr(0x0),
		colorBlendMode(BlendMode::Normal),
		currentDrawColor(colors::White)
	{
	}

	/** Width and height constructor
	  */
	ImageBuffer(const uint16_t& W, const uint16_t& H, const uint8_t& ch = 3) :
		bGood(true),
		nWidth(W),
		nHeight(H),
		nChannels(ch),
		nSize(W* H),
		nBytes(nSize* ch), // ch Color channels
		bitmap(nBytes, 0),
		dptr(&bitmap[0]),
		colorBlendMode(BlendMode::Normal),
		currentDrawColor(colors::White)
	{
	}

	/** Copy constructor
	  */
	ImageBuffer(const ImageBuffer&) = delete;

	/** Assignment operator
	  */
	ImageBuffer& operator = (const ImageBuffer&) = delete;

	/** Destructor
	  */
	~ImageBuffer();

	/** Return true if image data is loaded in conventional memory and return false otherwise
	  */
	bool Good() const {
		return bGood;
	}

	/** Get a const pointer to color data for a specified pixel
	  */
	const uint8_t* operator [] (const uint32_t& pixel) const {
		return const_cast<uint8_t*>(&dptr[pixel * nChannels]);
	}

	/** Get a pointer to color data for a specified pixel
	  */
	uint8_t* operator [] (const uint32_t& pixel) {
		return &dptr[pixel * nChannels];
	}

	/** Get a const pointer to color data for a specified pixel
	  */
	const uint8_t* Get(const uint16_t& x, const uint16_t& y) const {
		return const_cast<uint8_t*>(&dptr[(nWidth * y + x) * nChannels]);
	}

	/** Get a pointer to color data for a specified pixel
	  */
	uint8_t* Get(const uint16_t& x, const uint16_t& y) {
		return &dptr[(nWidth * y + x) * nChannels];
	}

	/** Get a const pointer to start of image data
	  */
	const uint8_t* Get() const {
		return const_cast<uint8_t*>(&dptr[0]);
	}

	/** Get a pointer to start of image data
	  */
	uint8_t* Get() {
		return &dptr[0];
	}

	/** Get a logical pixel from a pixel in the loaded image
	  * @return True if the specified pixel is a valid location
	  */
	bool GetPixel(const int32_t& x, const int32_t& y, LogicalColor& color);

	/** Get a pointer to the start of a pixel color data
	  * @return A pointer to the start of the pixel's color data, or return NULL if the location is invalid
	  */
	const uint8_t* GetPixel(const int32_t& x, const int32_t& y) const;

	/** Copy image buffer pixel data to a vector.
	  * Vector will be cleared before filling with pixel data.
	  */
	void CopyImageData(std::vector<uint8_t>& output) const;

	/** Get the true width of the loaded image (in pixels)
	  */
	uint16_t Width() const {
		return nWidth;
	}

	/** Get the true height of the loaded image (in pixels)
	  */
	uint16_t Height() const {
		return nHeight;
	}

	/** Get the number of pixel color channels
	  */
	uint16_t NumChannels() const {
		return nChannels;
	}

	/** Get the total number of image pixels
	  */
	uint32_t NumPixels() const {
		return nSize;
	}

	/** Get the total number of bytes of image data
	  */
	uint32_t NumBytes() const {
		return nBytes;
	}

	/** Return true if buffer data is empty
	  */
	bool Empty() const {
		return (nSize == 0);
	}

	/** Resize image buffer
	  */
	void Resize(const uint16_t& W, const uint16_t& H, const uint16_t& ch);

	/** Set the current pixel color blending mode. The default blending mode is
	  * BlendMode::NORMAL, which simply replaces the existing color with a new one.
	  */
	void SetBlendMode(const BlendMode& mode = BlendMode::Normal) {
		colorBlendMode = mode;
	}

	/** Set the draw color
	  */
	void SetDrawColor(const ColorRGB& color) {
		currentDrawColor = color;
	}

	/** Set the color of a pixel in the buffer, ignoring the current color blending mode
	  */
	void SetPixel(const uint16_t& x, const uint16_t& y, const ColorRGB& color);

	/** Set the color of a pixel in the buffer, ignoring the current color blending mode
	  */
	void SetPixel(const uint16_t& x, const uint16_t& y, const LogicalColor& color);

	/** Set the color of a horizontal row of pixels, ignoring the current color blending mode
	  */
	void SetPixelRow(const uint16_t& y, const ColorRGB& color);

	/** Set multiplicitive opacity of the texture
	  * @param opacity Opacity of texture, in range [0, 1]
	  */
	void SetOpacity(const float& opacity);

	/** Invert image colors
	  */
	void InvertColors();

	/** Convert image to grayscale based on sRGB convention
	  */
	void ToGrayscale();

	/** Set all pixel alpha values to one (i.e. remove alpha channel). This may not be undone
	  */
	void RemoveAlpha();

	/** Set a color in the image to translucent
	  * @param chroma Color to make translucent (chroma key)
	  * @param margin Margin of error for color match (0 is exact match, 1 accepts any color)
	  * @param opacity Opacity of translucent pixels (default is 0, i.e. fully transparent)
	  */
	void ColorToAlpha(const ColorRGB& chroma, const float& margin = 0, const float& opacity = 0);

	/** Fill the image with a color
	  * @param color Color to fill the image with
	  */
	void FillColor(const ColorRGB& color);

	/** Draw an external image buffer into a region of this buffer with upper left corner at (x,y)
	  */
	void BlendSubImage(const uint16_t& x, const uint16_t& y, ImageBuffer* buffer, const float& alpha);

	/** Draw an external image buffer into a region of this buffer with upper left corner at (x,y)
	  */
	void BlendSubImage(const uint16_t& x, const uint16_t& y, ImageBuffer* buffer);

	/** Copy an external image buffer into a region of this buffer with upper left corner at (x,y)
	  * This method expects the external image buffer to be smaller than this buffer. All pixels from
	  * the external buffer will be copied into this one starting at (x,y).
	  * See also drawPixels.
	  */
	void DrawSubImage(const uint16_t& x, const uint16_t& y, ImageBuffer* buffer);

	/** Draw a pixel at the position (x,y)
	  */
	void DrawPixel(const uint16_t& x, const uint16_t& y);

	/** Draw a series of (x,y) pairs
	  */
	void DrawPixels(const std::vector<std::pair<uint16_t, uint16_t> >& vertices);

	/** Draw a series of N (x,y) pairs.
	  * Input vertex array should be an ordered list of X and Y pairs and must be at least 2 * N elements long.
	  */
	void DrawPixels(const uint16_t* vertices, const size_t& N);

	/** Copy a region of an external image buffer with upper left corner at (x,y) into this one.
	  * This method expects the external image buffer to be larger than this buffer. A region of pixels
	  * with the width and height of this buffer will be extracted from the larger external buffer.
	  * See also drawSubImage.
	  */
	void DrawPixels(const uint16_t& x, const uint16_t& y, ImageBuffer* buffer);

	/** Draw a line between the two points (x0,y0) and (x1,y1)
	  */
	void DrawLine(
		const uint16_t& x0, const uint16_t& y0,
		const uint16_t& x1, const uint16_t& y1
	);

	/** Draw a sequence of lines between a series of (x,y) pairs
	  */
	void DrawLines(const std::vector<std::pair<uint16_t, uint16_t> >& vertices);

	/** Draw a sequence of lines between a series of N (x,y) pairs.
	  * Input vertex array should be an ordered list of X and Y pairs and must be at least 2 * N elements long.
	  */
	void DrawLines(const uint16_t* vertices, const size_t& N);

	/** Draw a closed polygon with N sides using a series of N (x,y) pairs
	  */
	void DrawPolygon(const std::vector<std::pair<uint16_t, uint16_t> >& vertices);

	/** Draw a closed polygon with N sides.
	  * Input vertex array should be an ordered list of X and Y pairs and must be at least 2 * N elements long.
	  */
	void DrawPolygon(const uint16_t* vertices, const size_t& N);

	/** Draw a rectangle with top left corner (x0,y0) and bottom right corner (x1,y1)
	  */
	void DrawRectangle(
		const uint16_t& x0, const uint16_t& y0,
		const uint16_t& x1, const uint16_t& y1,
		bool bFilled = false
	);

	/** Draw a circle with center (x0,y0) and specified pixel radius
	  */
	void DrawCircle(const float& x0, const float& y0, const float& radius, bool bFilled = false);

	/** Draw an ellipse with a specified center (x0,y0), semi-major radius (A), and semi-minor radius (B)
	  */
	void DrawEllipse(const uint16_t& x0, const uint16_t& y0, const float& A, const float& B, bool bFilled = false);

	/** Draw a regular N-sided polygon centered at (x0,y0) and whose vertices are circumscribed by a circle with the specified radius
	  */
	void DrawRegularPolygon(const uint16_t& x0, const uint16_t& y0, const uint16_t& N, const float& radius);

	/** Draw an arbitrary triangle with three specified vertices (x0,y0), (x1,y1), and (x2,y2)
	  */
	void DrawTriangle(
		const uint16_t& x0, const uint16_t& y0,
		const uint16_t& x1, const uint16_t& y1,
		const uint16_t& x2, const uint16_t& y2
	);

	/** Draw an arbitrary quad with four specified vertices (x0,y0), (x1,y1), (x2,y2), and (x3,y3)
	  */
	void DrawQuad(
		const uint16_t& x0, const uint16_t& y0,
		const uint16_t& x1, const uint16_t& y1,
		const uint16_t& x2, const uint16_t& y2,
		const uint16_t& x3, const uint16_t& y3
	);

	/** Manipulate the loaded image on the pixel level
	  * @param func Function pointer which will be called for every pixel in the image
	  */
	void ProcessImage(ImageManipulationFunc_t func);

	/** Clear image buffer to a specified color
	  * For monochromatic colors, it should be faster to use fill().
	  */
	void Clear(const ColorRGB& color);

	/** Fill image buffer with a value
	  * Uses std::fill to fill entire buffer with specified color component value.
	  */
	virtual void Fill(const uint8_t& value = 0);

	/** Free conventional memory being used to store image
	  */
	virtual void Free();

	/** Get a vector of pointers of pairs representing the destination and source of pixel data for all rows inside
	  * a bounding box with upper left corner at pixel location (x,y) in the destination image, at (0,0) in the
	  * source image, and with width and height taken from the source image.
	  * This allows for fast image data copying, assuming the color depth is the same for both images.
	  * @return The number of rows which may be safely copied, given a width of W pixels
	  */
	static size_t GetImageTargets(
		const int32_t& x, const int32_t& y,
		ImageBuffer* dest,
		ImageBuffer* src,
		std::vector<std::pair<uint8_t*, uint8_t*> >& targets
	);

	/** Get a vector of pointers of pairs representing the destination and source of pixel data for all rows inside
	  * a bounding box with upper left corner at pixel location (x0,y0) in the destination image, at (x1,y1) in the
	  * source image, and with width W and height H (both in pixels).
	  * This allows for fast image data copying, assuming the color depth is the same for both images.
	  * @return The number of rows which may be safely copied, given a width of W pixels
	  */
	static size_t GetImageTargets(
		const int32_t& x0, const int32_t& y0,
		const int32_t& x1, const int32_t& y1,
		const int32_t& W, const int32_t& H,
		ImageBuffer* dest,
		ImageBuffer* src,
		std::vector<std::pair<uint8_t*, uint8_t*> >& targets
	);

protected:
	bool bGood; ///< Set if image buffer memory allocated successfully

	uint16_t nWidth; ///< Width of image buffer in pixels

	uint16_t nHeight; ///< Height of image buffer in pixels

	uint8_t nChannels; ///< Number of color channels

	uint32_t nSize; ///< Total size of image buffer in pixels

	uint32_t nBytes; ///< Total size of image in bytes

	std::vector<uint8_t> bitmap; ///< Image data

	uint8_t* dptr; ///< Internal pointer to image data

	BlendMode colorBlendMode; ///< The current pixel blend mode

	ColorRGB currentDrawColor; ///< Current drawing color for pixel color blending operations

	/** Blend the current draw color into the image
	  */
	void BlendPixel(const uint16_t& px, const uint16_t& py);

	/** Blend a single pixel color into the image
	  */
	void BlendPixel(const uint16_t& px, const uint16_t& py, const ColorRGB& color);

	/** Blend a single pixel color into the image
	  */
	void BlendPixel(LogicalColor& dest, const ColorRGB& color) const;

	/** Blend a single logical pixel color into the image
	  */
	void BlendPixel(LogicalColor& dest, const LogicalColor& src) const;
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_IMAGE_BUFFER_HPP
