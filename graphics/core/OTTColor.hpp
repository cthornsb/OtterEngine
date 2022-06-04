#ifndef GRAPHICS_OTT_COLOR_RGB_HPP
#define GRAPHICS_OTT_COLOR_RGB_HPP

#include <cstdint>

namespace ott {

class ColorRGB{
public:
	uint8_t r; ///< Red color component
	uint8_t g; ///< Green color component
	uint8_t b; ///< Blue color component
	uint8_t a; ///< Alpha channel

	/** Default constructor (black)
	  */
	ColorRGB() : 
		r(0), 
		g(0), 
		b(0),
		a(255)
	{ 
	}

	/** Grayscale constructor (0, 1)
	  */
	ColorRGB(const float &value, const float& alpha=1.f);

	/** RGB constructor (0, 1)
	  */
	ColorRGB(const float &red, const float &green, const float &blue, const float& alpha=1.f);

	/** Get a reference to one of the four color components (RGBA)
	  */
	uint8_t& operator [] (const uint8_t& index);

	/** Get a const reference to one of the four color components (RGBA)
	  */
	uint8_t operator [] (const uint8_t& index) const ;

	/** Equality operator
	  */
	bool operator == (const ColorRGB &rhs) const { 
		return (r==rhs.r && g==rhs.g && b==rhs.b); 
	}

	/** Inequality operator
	  */
	bool operator != (const ColorRGB &rhs) const { 
		return (r!=rhs.r || g!=rhs.g || b!=rhs.b); 
	}

	/** Add a color to this color and return the result
	  */
	ColorRGB operator + (const ColorRGB &rhs) const ;

	/** Subtract a color from this color and return the result
	  */
	ColorRGB operator - (const ColorRGB &rhs) const ;

	/** Multiply this color by a constant scaling factor and return the result
	  */
	ColorRGB operator * (const float &rhs) const ;

	/** Divide this color by a constant scaling factor and return the restul
	  */
	ColorRGB operator / (const float &rhs) const ;

	/** Add a color to this color
	  */
	ColorRGB& operator += (const ColorRGB &rhs);

	/** Subtract a color from this color
	  */
	ColorRGB& operator -= (const ColorRGB &rhs);

	/** Multiply this color by a constant scaling factor
	  */
	ColorRGB& operator *= (const float &rhs);
	
	/** Divide this color by a constant scaling factor
	  */
	ColorRGB& operator /= (const float &rhs);

	/** Get the RGB inverse of this color
	  */
	ColorRGB Invert() const ;

	/** Conver the color to grayscale using RGB coefficients based on the sRGB convention
	  */
	void ToGrayscale();
	
	/** Dump the RGB color components to stdout
	  */
	void Dump() const ;
	
	/** Convert an input value to a grayscale heatmap
	  */
	static ColorRGB HeatMap(const float& x, const float& xmax, const float& xmin=0);
	
	/** Convert a floating point value in the range [0, 1] to a byte
	  */
	static uint8_t ToUChar(const float &val){ 
		return ((uint8_t)(val*255)); 
	}
	
	/** Convert a byte to a floating point value in the range [0, 1]
	  */
	static float ToFloat(const uint8_t &val){ 
		return (float(val)/255);
	}
};

namespace colors {

// Grayscale colors
extern const ColorRGB Black;
extern const ColorRGB DkGray;
extern const ColorRGB LtGray;
extern const ColorRGB White;

// Primary colors
extern const ColorRGB Red;
extern const ColorRGB Green;
extern const ColorRGB Blue;

// Secondary colors
extern const ColorRGB Yellow;
extern const ColorRGB Magenta;
extern const ColorRGB Cyan;

// Tertiary colors
extern const ColorRGB Orange;
extern const ColorRGB Chartreuse;
extern const ColorRGB Spring;
extern const ColorRGB Azure;
extern const ColorRGB Violet;
extern const ColorRGB Rose;

// Other
extern const ColorRGB Transparent;

} /* namespace ott::colors */
} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_COLOR_RGB_HPP

