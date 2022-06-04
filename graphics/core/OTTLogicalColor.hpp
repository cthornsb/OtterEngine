#ifndef GRAPHICS_OTT_LOGICAL_COLOR_HPP
#define GRAPHICS_OTT_LOGICAL_COLOR_HPP

#include <cstdint>

namespace ott {

class ColorRGB;

class LogicalColor {
public:
	uint8_t* pArray; ///< Start of color component array

	bool bAlpha; ///< Set if each pixel contains an alpha channel

	/** Default constructor (empty)
	  */
	LogicalColor(bool alpha = true) :
		pArray(0x0),
		bAlpha(alpha)
	{
	}

	/** Array constructor. Expects at least four elements
	  */
	LogicalColor(uint8_t* arr) :
		pArray(arr),
		bAlpha(true)
	{
	}

	bool operator == (const ColorRGB& rhs) const;

	/** Pixel color element access
	  */
	uint8_t& operator [] (const uint8_t& index) {
		return pArray[index];
	}

	/** Const pixel color element access
	  */
	uint8_t operator [] (const uint8_t& index) const {
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

	/** Add a color to this color
	  * @note Alpha is set to 1
	  */
	void operator += (const LogicalColor& rhs);

	/** Subtract a color from this color
	  * @note Alpha is set to 1
	  */
	void operator -= (const LogicalColor& rhs);

	/** Multiply this color by a constant scaling factor
	  * @note Alpha is set to 1
	  */
	void operator *= (const LogicalColor& rhs);

	/** Divide this color by a constant scaling factor
	  * @note Alpha is set to 1
	  */
	void operator /= (const LogicalColor& rhs);

	/** Set pixel data array pointer
	  */
	void operator = (uint8_t* rhs) {
		pArray = rhs;
	}

	/** Color assignment operator
	  */
	void operator = (const ColorRGB& rhs);

	/** Color assigment operator
	  */
	void operator = (const LogicalColor& rhs);

	/** Invert this color
	  * @note Alpha is preserved for each pixel
	  */
	void Invert() const;

	/** Convert the color to grayscale using RGB coefficients based on the sRGB convention
	  * @note Alpha is preserved for each pixel
	  */
	void ToGrayscale();

	/** Return the red component, scaled by the color's alpha channel
	  */
	float R() const {
		return pArray[0] * this->A() / 255.f;
	}

	/** Return the green component, scaled by the color's alpha channel
	  */
	float G() const {
		return pArray[1] * this->A() / 255.f;
	}

	/** Return the blue component, scaled by the color's alpha channel
	  */
	float B() const {
		return pArray[2] * this->A() / 255.f;
	}

	/** Return the alpha channel
	  */
	float A() const {
		return (bAlpha ? pArray[3] / 255.f : 1.f);
	}

	/** Get a copy of the current pixel color
	  */
	ColorRGB GetColor() const;
	//{ // todo
	//	return ColorRGB(getR(), getG(), getB(), getA());
	//}

	/** Set the red component
	  */
	void SetR(const float& value) {
		pArray[0] = LogicalColor::ClampUChar(value * 255);
	}

	/** Set the green component
	  */
	void SetG(const float& value) {
		pArray[1] = LogicalColor::ClampUChar(value * 255);
	}

	/** Set the blue component
	  */
	void SetB(const float& value) {
		pArray[2] = LogicalColor::ClampUChar(value * 255);
	}

	/** Set the alpha channel
	  */
	void SetA(const float& value) {
		if (bAlpha) {
			pArray[3] = LogicalColor::ClampUChar(value);
		}
	}

	/** Set the color
	  */
	void Set(const ColorRGB& color);

	/** Set the color
	  */
	void Set(const LogicalColor& color);

	/** Set the color from an input array of integers.
	  * Input array must contain at least four elements.
	  */
	void Set(const uint8_t* arr);

	/** Set the color with explicit RGBA components
	  */
	void Set(const float& r, const float& g, const float& b, const float& a = 1);

	/** Lighten the current color by taking the maximum color components between the current color and an input color
	  * @note Alpha is set to 1
	  */
	void Lighten(const ColorRGB& color);

	/** Darken the current color by taking the minimum color components between the current color and an input color
	  * @note Alpha is set to 1
	  */
	void Darken(const ColorRGB& color);

	/** Compute the difference between each color component of the current color and an input color. The result is guaranteed to be positive
	  * @note Alpha is set to 1
	  */
	void Difference(const ColorRGB& color);

	/** Compute the average between each color component of the current color and an input color, scaled by its alpha value
	  */
	void Average(const ColorRGB& color);

	/** Lighten the current color by taking the maximum color components between the current color and an input color
	  * @note Alpha is set to 1
	  */
	void Lighten(const LogicalColor& color);

	/** Darken the current color by taking the minimum color components between the current color and an input color
	  * @note Alpha is set to 1
	  */
	void Darken(const LogicalColor& color);

	/** Compute the difference between each color component of the current color and an input color. The result is guaranteed to be positive
	  * @note Alpha is set to 1
	  */
	void Difference(const LogicalColor& color);

	/** Compute the average between each color component of the current color and an input color, scaled by its alpha value
	  */
	void Average(const LogicalColor& color);

	/** Compare this color to an input color within a given margin of error
	  */
	bool CompareColor(const ColorRGB& color, const float& margin = 0.0f);

	/** Dump the RGB color components to stdout
	  */
	void Dump() const;

	/** Reset color to fully transparent black
	  */
	void Reset();

	/** Clamp input float value to the range 0 to 255 and return as unsigned integer
	  */
	static uint8_t ClampUChar(const float& value);

	/** Clamp input float value to the range 0 to 1 and return as float
	  */
	static float ClampFloat(const float& value);

	static float GetComponentWithOpacity(const uint8_t& comp, const uint8_t& opacity);
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_LOGICAL_COLOR_HPP
