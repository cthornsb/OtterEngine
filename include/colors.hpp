#ifndef COLORS_HPP
#define COLORS_HPP

class sdlColor{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;

	/** Default constructor (black)
	  */
	sdlColor() : r(0), g(0), b(0) { }

	/** Grayscale constructor (0, 1)
	  */
	sdlColor(const float &value);

	/** RGB constructor (0, 1)
	  */
	sdlColor(const float &red, const float &green, const float &blue);

	/** Conver the color to grayscale using RGB coefficients based on the sRGB convention
	  */
	void toGrayscale();
	
	/** Dump the RGB color components to stdout
	  */
	void dump() const ;
	
	/** Convert a floating point value in the range [0, 1] to an unsigned char between 0 and 255
	  */
	static unsigned char convertToUChar(const float &val){ return ((unsigned char)(val*255)); }
};

namespace Colors{
	const sdlColor BLACK;
	const sdlColor WHITE(1, 1, 1);
	
	// Primary colors
	const sdlColor RED(1, 0, 0);
	const sdlColor GREEN(0, 1, 0);
	const sdlColor BLUE(0, 0, 1);
	
	// Secondary colors
	const sdlColor YELLOW(1, 1, 0);
	const sdlColor MAGENTA(1, 0, 1);
	const sdlColor CYAN(0, 1, 1);
}

#endif
