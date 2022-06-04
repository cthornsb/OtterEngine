#include "OTTAnalogStick.hpp"

void ott::AnalogStick::Set(const float* data) {
	fDX = (data[nX] - fOffsetX) - fX;
	fDY = (data[nY] - fOffsetY) - fY;
	fX = (data[nX] - fOffsetX);
	fY = (data[nY] - fOffsetY);
	fDist = fX * fX + fY * fY;
}

void ott::AnalogStick::Set(const float& x, const float& y) {
	fDX = (x - fOffsetX) - fX;
	fDY = (y - fOffsetX) - fY;
	fX = (x - fOffsetX);
	fY = (y - fOffsetY);
	fDist = fX * fX + fY * fY;
}

void ott::AnalogStick::SetCalibration(const float& x, const float& y) {
	fOffsetX = x;
	fOffsetY = y;
}
