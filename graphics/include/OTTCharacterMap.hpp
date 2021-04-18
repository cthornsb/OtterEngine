#ifndef OTT_CHARACTER_MAP_HPP
#define OTT_CHARACTER_MAP_HPP

#include <string>
#include <vector>

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

	/** Font bitmap filename constructor with character bitmap width, height, and decoder function pointer
	  */
	OTTCharacterMap(
		const std::string& fname, 
		const unsigned short& W, 
		const unsigned short& H, 
		const unsigned short& bitsPerPixel, 
		OTTBitmap::bitmapDecodeFunction func
	);

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
		foreground = color;
	}

	/** Set the font foreground color
	  */
	void setBackgroundColor(const ColorRGB& color) {
		background = color;
	}

	/** Enable or disable text background color
	  */
	void setTransparency(bool state = true) {
		bTransparency = state;
	}

	/** Load ascii character bitmaps from a file
	  */
	bool loadCharacterBitmaps(const std::string& fname, const unsigned short& nBitsPerPixel = 1, OTTBitmap::bitmapDecodeFunction func = 0x0);

	/** Print a character on the screen at a specified location
	  * Locations are indexed from (0,0) at top left of screen
	  * @param val The character to print
	  * @param x Column where character will be printed
	  * @param y Row where character will be printed
	  */
	void putCharacter(const char& val, const unsigned short& x, const unsigned short& y);

	/** Print a string on the screen at a specified location
	  * Locations are indexed from (0,0) at top left of screen
	  * @param str The string to print
	  * @param x Column where first character will be printed
	  * @param y Row where first character will be printed
	  * @param wrap If set to true, text extending off the screen will wrap to a new line
	  */
	void putString(const std::string& str, const unsigned short& x, const unsigned short& y, bool wrap = true);

	/** Draw character string to external RAM image buffer with upper left corner at pixel offset of (x0, y0)
	  */
	void drawString(
		const std::string& str,
		const unsigned short& x0,
		const unsigned short& y0,
		OTTImageBuffer* buffer,
		const unsigned char& alphaColor = 4,
		bool invert = false
	);

protected:
	OTTWindow* window; ///< Pointer to graphical display window

	bool bTransparency; ///< Set if text background transparency is enabled

	bool bInvertColors; ///< Set if pixel colors should be inverted

	unsigned short nWidth; ///< Width of bitmaps in pixels

	unsigned short nHeight; ///< Height of bitmaps in pixels

	unsigned short nLength; ///< Number of pixels per bitmap

	ColorRGB foreground; ///< Text color foreground

	ColorRGB background; ///< Text color foreground

	std::vector<OTTBitmap> cmap; ///< Ascii character map bitmaps
};

#endif // ifndef OTT_CHARACTER_MAP_HPP