#include "OTTWrappedValue.hpp"

#include <cmath>

float ott::WrappedValue::Wrap(const float& d) const {
	float retval = value + d;
	if (!lock) {
		retval = fmod((value + d) - minVal, delta);
		if (retval < 0) {
			if (!lock)
				retval += delta;
			else
				retval = 0;
		}
		return retval + minVal;
	}
	else if (retval < minVal)
		return minVal;
	else if (retval > maxVal)
		return maxVal;
	return retval;
}
