#ifndef LOGICAL_COLOR_HPP
#define LOGICAL_COLOR_HPP

#include "ColorRGB.hpp"

unsigned char limitValueUChar(const float& value);

float limitValueFloat(const float& value);

class LogicalColor {
public:
	unsigned char* pArray; ///< Start of color component array

	/** Default constructor (empty)
	  */
	LogicalColor() :
		pArray(0x0)
	{
	}

	/** Array constructor. Expects at least four elements
	  **/
	LogicalColor(unsigned char* arr) :
		pArray(arr)
	{
	}

	bool operator == (const ColorRGB& rhs) const;

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
	float getR() const { return pArray[0] * pArray[3] / 65025.f; }

	/** Return the green component, scaled by the color's alpha channel
	  **/
	float getG() const { return pArray[1] * pArray[3] / 65025.f; }

	/** Return the blue component, scaled by the color's alpha channel
	  **/
	float getB() const { return pArray[2] * pArray[3] / 65025.f; }

	/** Return the alpha channel
	  **/
	float getA() const { return pArray[3] / 255.f; }

	/** Set the red component
	  **/
	void setRed(const float& value) { pArray[0] = limitValueUChar(value * 255); }

	/** Set the green component
	  **/
	void setGreen(const float& value) { pArray[1] = limitValueUChar(value * 255); }

	/** Set the blue component
	  **/
	void setBlue(const float& value) { pArray[2] = limitValueUChar(value * 255); }

	/** Set the alpha channel
	  **/
	void setAlpha(const float& value) { pArray[3] = limitValueUChar(value * 255); }

	/** Set the color
	  **/
	void setColor(const ColorRGB& color);

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
};

#endif