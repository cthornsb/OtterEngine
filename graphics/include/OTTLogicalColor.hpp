#ifndef OTT_LOGICAL_COLOR_HPP
#define OTT_LOGICAL_COLOR_HPP

#include "ColorRGB.hpp"

class OTTLogicalColor {
public:
	unsigned char* pArray; ///< Start of color component array

	bool bAlpha; ///< Set if each pixel contains an alpha channel

	/** Default constructor (empty)
	  */
	OTTLogicalColor(bool alpha = true) :
		pArray(0x0),
		bAlpha(alpha)
	{
	}

	/** Array constructor. Expects at least four elements
	  **/
	OTTLogicalColor(unsigned char* arr) :
		pArray(arr),
		bAlpha(true)
	{
	}

	bool operator == (const ColorRGB& rhs) const;

	/** Pixel color element access
	  */
	unsigned char& operator [] (const unsigned char& index){
		return pArray[index];
	}

	/** Const pixel color element access
	  */
	unsigned char operator [] (const unsigned char& index) const {
		return pArray[index];
	}

	/** Add a color to this color (ignoring alpha) and return the result
	  */
	ColorRGB operator + (const ColorRGB& rhs) const;

	/** Subtract a color from this color (ignoring alpha) and return the result
	  */
	ColorRGB operator - (const ColorRGB& rhs) const;

	/** Multiply this color by a constant scaling factor (ignoring alpha) and return the result
	  */
	ColorRGB operator * (const float& rhs) const;

	/** Divide this color by a constant scaling factor (ignoring alpha) and return the result
	  */
	ColorRGB operator / (const float& rhs) const;

	/** Add a color to this color
	  * @note Alpha is set to 1
	  */
	void operator += (const ColorRGB& rhs);

	/** Subtract a color from this color
	  * @note Alpha is set to 1
	  */
	void operator -= (const ColorRGB& rhs);

	/** Multiply this color by a constant scaling factor
	  * @note Alpha is set to 1
	  */
	void operator *= (const ColorRGB& rhs);

	/** Divide this color by a constant scaling factor
	  * @note Alpha is set to 1
	  */
	void operator /= (const ColorRGB& rhs);

	/** Set pixel data array pointer
	  */
	void operator = (unsigned char* rhs){
		pArray = rhs;
	}

	/** Color assignment operator
	  */
	void operator = (const ColorRGB& rhs);
	
	/** Color assigment operator
	  */
	void operator = (const OTTLogicalColor& rhs);

	/** Invert this color
	  * @note Alpha is preserved for each pixel
	  */
	void invert() const;

	/** Convert the color to grayscale using RGB coefficients based on the sRGB convention
	  * @note Alpha is preserved for each pixel
	  */
	void toGrayscale();

	/** Return the red component, scaled by the color's alpha channel
	  **/
	float getR() const {
		return pArray[0] * getA() / 255.f;
	}

	/** Return the green component, scaled by the color's alpha channel
	  **/
	float getG() const {
		return pArray[1] * getA() / 255.f;
	}

	/** Return the blue component, scaled by the color's alpha channel
	  **/
	float getB() const {
		return pArray[2] * getA() / 255.f;
	}

	/** Return the alpha channel
	  **/
	float getA() const {
		if(bAlpha)
			return pArray[3] / 255.f;
		return 1.f;
	}
	
	/** Get a copy of the current pixel color
	  */
	ColorRGB getColor() const {
		return ColorRGB(pArray[0] / 255.f, pArray[1] / 255.f, pArray[2] / 255.f);
	}

	/** Set the red component
	  **/
	void setRed(const float& value) {
		pArray[0] = clampUChar(value * 255);
	}

	/** Set the green component
	  **/
	void setGreen(const float& value) {
		pArray[1] = clampUChar(value * 255);
	}

	/** Set the blue component
	  **/
	void setBlue(const float& value) {
		pArray[2] = clampUChar(value * 255);
	}

	/** Set the alpha channel
	  **/
	void setAlpha(const float& value) {
		if (bAlpha)
			pArray[3] = clampUChar(value * 255);
	}

	/** Set the color
	  **/
	void setColor(const ColorRGB& color);

	/** Set the color
	  */
	void setColor(const OTTLogicalColor& color);

	/** Set the color from an input array of integers.
	  * Input array must contain at least four elements.
	  */
	void setColor(const unsigned char* arr);

	/** Set the color with explicit RGBA components
	  **/
	void setColor(const float& r, const float& g, const float& b, const float& a = 1);

	/** Lighten the current color by taking the maximum color components between the current color and an input color
	  * @note Alpha is set to 1
	  **/
	void lighten(const ColorRGB& color);

	/** Darken the current color by taking the minimum color components between the current color and an input color
	  * @note Alpha is set to 1
	  **/
	void darken(const ColorRGB& color);

	/** Compute the difference between each color component of the current color and an input color. The result is guaranteed to be positive
	  * @note Alpha is set to 1
	  **/
	void difference(const ColorRGB& color);

	/** Compare this color to an input color within a given margin of error
	  **/
	bool compareColor(const ColorRGB& color, const float& margin = 0.0f);

	/** Dump the RGB color components to stdout
	  */
	void dump() const;

	/** Reset color to fully transparent black
	  */
	void reset();
	
	/** Clamp input float value to the range 0 to 255 and return as unsigned integer
	  */
	static unsigned char clampUChar(const float& value);

	/** Clamp input float value to the range 0 to 1 and return as float
	  */
	static float clampFloat(const float& value);
};

#endif
