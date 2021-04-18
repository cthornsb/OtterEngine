#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <vector>

class OTTBitmap {
public:
	typedef unsigned char (*bitmapDecodeFunction)(const unsigned short&, const unsigned short&, const unsigned char*);

	/** Default constructor
	  */
	OTTBitmap();

	/** Explicit pixel color constructor (loose-packing)
	  * Input array should have AT LEAST 64 elements
	  */
	OTTBitmap(const int& W, const int& H, const int& bits, unsigned char* bmp);

	/** Return true if the color of at least one pixel in the bitmap is non-zero
	  */
	bool isBlank() const {
		return bBlank;
	}

	/** Get the color of the pixel at location (x,y) indexed from (0,0) at the top left
	  */
	unsigned char get(const unsigned short& x, const unsigned short& y) const;

	/** Set the color of the pixel at location (x,y) indexed from (0,0) at the top left
	  */
	void set(const unsigned short& x, const unsigned short& y, const unsigned char& color);

	/** Set the color of all bitmap pixels
	  * @param data Array of tightly-packed pixel colors, expects AT LEAST 16 elements (two bytes per row)
	  */
	void set(const unsigned char* data);

	/** Set the color of all bitmap pixels
	  * @param data Array of tightly-packed pixel colors, expects AT LEAST 16 elements (two bytes per row)
	  */
	void set(const unsigned char* data, bitmapDecodeFunction decoder);

	/** Dump all pixel colors to stdout
	  */
	void dump();

private:
	bool bBlank; ///< Unset if any pixel in the bitmap is non-zero

	unsigned short nWidth; ///< Width of bitmap in pixels

	unsigned short nHeight; ///< Height of bitmap in pixels

	unsigned char nBits; ///< Bitmap color depth bits

	std::vector<unsigned char> pixels; ///< Two-bit colors for all bitmap pixels
};

#endif // ifndef BITMAP_HPP