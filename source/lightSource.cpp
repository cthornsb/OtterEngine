#include <cmath>

#include "Globals.hpp"
#include "lightSource.hpp"
#include "triangle.hpp"

float lightSource::getIntensity(const pixelTriplet& surface) const {
	// Compute the dot-product between the triangle normal and the light direction
	// When the normal is close to anti-parallel with the light direction, the triangle 
	//  will receive close to the maximum light level. When the angle between the two
	//  vectors approaches 90 degrees (and above), the triangle will receive little to no light.
	float dp = -dir.cosTheta(surface.tri->norm);
	return (dp > 0 ? brightness*dp : 0);
}

float pointLight::getIntensity(const pixelTriplet& surface) const {
	vector3 displacement = (surface.getCenterPoint() - pos);
	float dp = -displacement.cosTheta(surface.tri->norm);
	if(dp < 0) 
		return 0;
	return (brightness * dp / displacement.square());
}

float coneLight::getIntensity(const pixelTriplet& surface) const {
	vector3 displacement = (surface.getCenterPoint() - pos);
	float dp = -displacement.cosTheta(surface.tri->norm);
	if (dp < 0)
		return 0;
	float beta = displacement.angle(dir);
	if(beta > openingAngle/2)
		return 0;
	return (brightness * dp / displacement.square());
}
