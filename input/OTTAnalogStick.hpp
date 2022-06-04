#ifndef INPUT_OTT_ANALOG_STICK_HPP
#define INPUT_OTT_ANALOG_STICK_HPP

#include <cstdint>

namespace ott {

class AnalogStick {
public:
	float fX; ///< Analog stick x position (-1, 1)

	float fY; ///< Analog stick y position (-1, 1)

	float fDX; ///< Change in analog x position since last position update

	float fDY; ///< Change in analog y position since last position update

	float fDist; ///< Analog stick squared "distance" from center

	/** Default constructor
	  */
	AnalogStick() = delete;

	/** Array index constructor
	  */
	AnalogStick(const uint8_t& xIndex, const uint8_t& yIndex) :
		fX(0.f),
		fY(0.f),
		fDX(0.f),
		fDY(0.f),
		fDist(0.f),
		nX(xIndex),
		nY(yIndex),
		fOffsetX(0.f),
		fOffsetY(0.f)
	{
	}

	/** Set analog stick X and Y positions from an input data array
	  */
	void Set(const float* data);

	/** Set analog stick X and Y positions from explicit floating point values
	  */
	void Set(const float& x, const float& y);

	/** Set analog stick center point offset calibration
	  */
	void SetCalibration(const float& x, const float& y);

private:
	uint8_t nX; ///< Data array index for X value

	uint8_t nY; ///< Data array index for Y value

	float fOffsetX; ///< Analog stick center x calibration offset

	float fOffsetY; ///< Analog stick center y calibration offset
};

} /* namespace ott */

#endif // #ifndef INPUT_OTT_ANALOG_STICK_HPP
