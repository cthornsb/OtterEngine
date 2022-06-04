#ifndef GRAPHICS_OTT_CHARACTER_MAP_HPP
#define GRAPHICS_OTT_CHARACTER_MAP_HPP

#include <graphics/core/OTTColor.hpp>
#include <graphics/texture/OTTBitmap.hpp>

#include <fstream>
#include <cstdint>
#include <string>
#include <vector>

namespace ott {

class Window;

class ImageBuffer;

class CharacterMap {
public:
	enum class TextAlignment {
		TopLeft,
		TopCenter,
		TopRight,
		MiddleLeft,
		MiddleCenter,
		MiddleRight,
		BottomLeft,
		BottomCenter,
		BottomRight
	};

	/** Default constructor
	  */
	CharacterMap() = delete;

	/** Font bitmap filename constructor
	  */
	CharacterMap(const std::string& fname);

	/** Font bitmap filename constructor with explicit character bitmap width and height (in pixels)
	  */
	CharacterMap(const std::string& fname, const uint16_t& W, const uint16_t& H);

	/** Font bitmap filename constructor with character bitmap width, height, pixel color depth, and decoder function pointer
	  */
	CharacterMap(
		const std::string& fname, 
		const uint16_t& W, 
		const uint16_t& H, 
		const uint16_t& bitsPerPixel, 
		Bitmap::BitmapDecodeFunction_t func
	);

	/** Font bitmap filename constructor with character bitmap width, height, and pixel color depth
	  */
	CharacterMap(const uint16_t& W, const uint16_t& H, const uint16_t& bitsPerPixel);

	/** Get the width of all characters, in pixels
	  */
	uint16_t CharacterWidth() const {
		return nWidth;
	}

	/** Get the height of all characters, in pixels
	*/
	uint16_t CharacterHeight() const {
		return nHeight;
	}

	/** Set the graphical window to draw to
	  */
	void SetWindow(Window* win) {
		window = win;
	}

	/** Set the font foreground color
	  */
	void SetForegroundColor(const ColorRGB& color) {
		palette[nColorsPerPixel - 1] = color;
	}

	/** Set the font foreground color
	  */
	void SetBackgroundColor(const ColorRGB& color) {
		palette[0] = color;
	}

	/** Set the font foreground color
	  */
	void SetPaletteColor(const uint16_t& index, const ColorRGB& color) {
		if(index < nColorsPerPixel)
			palette[index] = color;
	}

	/** Enable or disable text background color
	  */
	void SetTransparency(bool state = true) {
		bTransparency = state;
	}

	/** Set text string output alignment
	  */
	void SetTextAlignment(const TextAlignment& alignment) {
		cursorAlignment = alignment;
	}

	/** Set text string output cursor position
	  */
	void SetCursorPosition(const uint16_t& px, const uint16_t& py) {
		nCursorX = px;
		nCursorY = py;
	}

	/** Load ascii character bitmaps from an input file
	  */
	bool LoadCharacterBitmaps(std::ifstream& ifile, Bitmap::BitmapDecodeFunction_t func = 0x0);

	/** Draw character string to external RAM image buffer with upper left corner at pixel offset of (x0, y0)
	  */
	void DrawString(
		const std::string& str,
		const uint16_t& x0,
		const uint16_t& y0,
		ImageBuffer* buffer,
		const uint8_t& alphaColor = 0xff,
		bool invert = false
	);

	/** Draw character string to external RAM image buffer at current cursor position.
	  * Vertical cursor position is automatically advanced by font height after line is printed.
	  */
	void DrawString(
		const std::string& str,
		ImageBuffer* buffer,
		const uint8_t& alphaColor = 0xff,
		bool invert = false
	);

protected:
	Window* window; ///< Pointer to graphical display window

	bool bTransparency; ///< Set if text background transparency is enabled

	bool bInvertColors; ///< Set if pixel colors should be inverted

	uint16_t nWidth; ///< Width of bitmaps in pixels

	uint16_t nHeight; ///< Height of bitmaps in pixels

	uint16_t nLength; ///< Number of pixels per bitmap

	uint16_t nBitsPerPixel; ///< Number of color bits per pixel (color depth)

	uint16_t nColorsPerPixel; ///< Number of possible colors per pixel (color depth)

	uint16_t nCursorX; ///< Current horizontal cursor position

	uint16_t nCursorY; ///< Current vertical cursor position

	TextAlignment cursorAlignment; ///< Current horizontal text alignment

	std::vector<ColorRGB> palette; ///< Font color palette

	std::vector<Bitmap> cmap; ///< Ascii character map bitmaps

	uint16_t StrWidthPixels(const std::string& str);
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_CHARACTER_MAP_HPP
