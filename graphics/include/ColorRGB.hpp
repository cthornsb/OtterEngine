#ifndef COLOR_RGB_HPP
#define COLOR_RGB_HPP

class ColorRGB{
public:
	unsigned char r; ///< Red color component
	unsigned char g; ///< Green color component
	unsigned char b; ///< Blue color component
	unsigned char a; ///< Alpha channel

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
	unsigned char& operator [] (const unsigned char& index);

	/** Get a const reference to one of the four color components (RGBA)
	  */
	unsigned char operator [] (const unsigned char& index) const ;

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
	ColorRGB invert() const ;

	/** Conver the color to grayscale using RGB coefficients based on the sRGB convention
	  */
	void toGrayscale();
	
	/** Dump the RGB color components to stdout
	  */
	void dump() const ;
	
	/** Convert an input value to a grayscale heatmap
	  */
	static ColorRGB heatMap(const float& x, const float& xmax, const float& xmin=0);
	
	/** Convert a floating point value in the range [0, 1] to an unsigned char between 0 and 255
	  */
	static unsigned char toUChar(const float &val){ 
		return ((unsigned char)(val*255)); 
	}
	
	/** Convert an unsigned char to a floating point value in the range [0, 1]
	  */
	static float toFloat(const unsigned char &val){ 
		return (float(val)/255);
	}
};

namespace Colors{
	// Grayscale colors
	extern const ColorRGB BLACK;
	extern const ColorRGB DKGRAY;
	extern const ColorRGB LTGRAY;
	extern const ColorRGB WHITE;
	
	// Primary colors
	extern const ColorRGB RED;
	extern const ColorRGB GREEN;
	extern const ColorRGB BLUE;
	
	// Secondary colors
	extern const ColorRGB YELLOW;
	extern const ColorRGB MAGENTA;
	extern const ColorRGB CYAN;
	
	// Tertiary colors
	extern const ColorRGB ORANGE;
	extern const ColorRGB CHARTREUSE;
	extern const ColorRGB SPRING;
	extern const ColorRGB AZURE;
	extern const ColorRGB VIOLET;
	extern const ColorRGB ROSE;
}

#endif
