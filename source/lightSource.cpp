#include <cmath>

#include "lightSource.hpp"
#include "plane.hpp"

float lightSource::getIntensity(const plane *surface) const {
	// Compute the dot-product between the triangle normal and the light direction
	// When the normal is close to anti-parallel with the light direction, the triangle 
	//  will receive close to the maximum light level. When the angle between the two
	//  vectors approaches 90 degrees (and above), the triangle will receive little to no light.
	float dp = -(dir * surface->norm);
	return (dp > 0 ? brightness*dp : 0);
}

float pointLight::getIntensity(const plane *surface) const {
	float dp = -(dir * surface->norm);
	if(dp < 0) 
		return 0;
	return dp/(surface->p - pos).length();
}

float coneLight::getIntensity(const plane *surface) const {
	float dp = -(dir * surface->norm);
	if(dp < 0) 
		return 0;
	vector3 displacement = (surface->p - pos);
	float dist = displacement.length();
	float beta = std::acos((displacement * dir)/dist);
	if(beta > openingAngle/2)
		return 0;
	return (dp / dist);
}
