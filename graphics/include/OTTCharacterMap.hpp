#ifndef OTT_CHARACTER_MAP_HPP
#define OTT_CHARACTER_MAP_HPP

#include <string>
#include <vector>
#include <fstream>

#include "ColorRGB.hpp"
#include "OTTBitmap.hpp"

class OTTWindow;

class OTTImageBuffer;

class OTTCharacterMap {
public:
	/** Default constructor
	  */
	OTTCharacterMap() = delete;

	/** Font bitmap filename constructor
	  */
	OTTCharacterMap(const std::string& fname);

	/** Font bitmap filename constructor with explicit character bitmap width and height (in pixels)
	  */
	OTTCharacterMap(const std::string& fname, const unsigned short& W, const unsigned short& H);

	/** Font bitmap filename constructor with character bitmap width, height, pixel color depth, and decoder function pointer
	  */
	OTTCharacterMap(
		const std::string& fname, 
		const unsigned short& W, 
		const unsigned short& H, 
		const unsigned short& bitsPerPixel, 
		OTTBitmap::bitmapDecodeFunction func
	);

	/** Font bitmap filename constructor with character bitmap width, height, and pixel color depth
	  */
	OTTCharacterMap(const unsigned short& W, const unsigned short& H, const unsigned short& bitsPerPixel);

	/** Get the width of all characters, in pixels
	  */
	unsigned short getCharacterWidth() const {
		return nWidth;
	}

	/** Get the height of all characters, in pixels
	*/
	unsigned short getCharacterHeight() const {
		return nHeight;
	}

	/** Set the graphical window to draw to
	  */
	void setWindow(OTTWindow* win) {
		window = win;
	}

	/** Set the font foreground color
	  */
	void setForegroundColor(const ColorRGB& color) {
		palette[nColorsPerPixel - 1] = color;
	}

	/** Set the font foreground color
	  */
	void setBackgroundColor(const ColorRGB& color) {
		palette[0] = color;
	}

	/** Set the font foreground color
	  */
	void setPaletteColor(const unsigned short& index, const ColorRGB& color) {
		if(index < nColorsPerPixel)
			palette[index] = color;
	}

	/** Enable or disable text background color
	  */
	void setTransparency(bool state = true) {
		bTransparency = state;
	}

	/** Load ascii character bitmaps from an input file.
	  * 
	  */
	bool loadCharacterBitmaps(std::ifstream& ifile, OTTBitmap::bitmapDecodeFunction func = 0x0);

	/** Draw character string to external RAM image buffer with upper left corner at pixel offset of (x0, y0)
	  */
	void drawString(
		const std::string& str,
		const unsigned short& x0,
		const unsigned short& y0,
		OTTImageBuffer* buffer,
		const unsigned char& alphaColor = 0xff,
		bool invert = false
	);

protected:
	OTTWindow* window; ///< Pointer to graphical display window

	bool bTransparency; ///< Set if text background transparency is enabled

	bool bInvertColors; ///< Set if pixel colors should be inverted

	unsigned short nWidth; ///< Width of bitmaps in pixels

	unsigned short nHeight; ///< Height of bitmaps in pixels

	unsigned short nLength; ///< Number of pixels per bitmap

	unsigned short nBitsPerPixel; ///< Number of color bits per pixel (color depth)

	unsigned short nColorsPerPixel; ///< Number of possible colors per pixel (color depth)

	std::vector<ColorRGB> palette; ///< Font color palette

	std::vector<OTTBitmap> cmap; ///< Ascii character map bitmaps
};

#endif // ifndef OTT_CHARACTER_MAP_HPP