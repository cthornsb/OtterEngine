#ifndef GRAPHICS_OTT_BITMAP_HPP
#define GRAPHICS_OTT_BITMAP_HPP

#include <cstdint>
#include <vector>

namespace ott {

class Bitmap {
public:
	typedef uint8_t (*BitmapDecodeFunction_t)(const uint16_t&, const uint16_t&, const uint8_t*);

	/** Default constructor
	  */
	Bitmap();

	/** Explicit pixel color constructor (loose-packing)
	  * Input array should have AT LEAST 64 elements
	  */
	Bitmap(const uint16_t& W, const uint16_t& H, const uint8_t& bits, uint8_t* bmp);

	/** Return true if the color of at least one pixel in the bitmap is non-zero
	  */
	bool Blank() const {
		return bBlank;
	}

	/** Get the color of the pixel at location (x,y) indexed from (0,0) at the top left
	  */
	uint8_t Get(const uint16_t& x, const uint16_t& y) const;

	/** Set the color of the pixel at location (x,y) indexed from (0,0) at the top left
	  */
	void Set(const uint16_t& x, const uint16_t& y, const uint8_t& color);

	/** Set the color of all bitmap pixels
	  * @param data Array of tightly-packed pixel colors, expects AT LEAST 16 elements (two bytes per row)
	  */
	void Set(const uint8_t* data);

	/** Set the color of all bitmap pixels
	  * @param data Array of tightly-packed pixel colors, expects AT LEAST 16 elements (two bytes per row)
	  */
	void Set(const uint8_t* data, BitmapDecodeFunction_t decoder);

	/** Dump all pixel colors to stdout
	  */
	void Dump();

private:
	bool bBlank; ///< Unset if any pixel in the bitmap is non-zero

	uint16_t nWidth; ///< Width of bitmap in pixels

	uint16_t nHeight; ///< Height of bitmap in pixels

	uint8_t nBits; ///< Bitmap color depth bits

	std::vector<uint8_t> pixels; ///< Two-bit colors for all bitmap pixels
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_BITMAP_HPP
