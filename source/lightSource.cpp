#include <cmath>

#include "Globals.hpp"
#include "lightSource.hpp"
#include "Vertex.hpp"

float lightSource::getIntensity(const Vertex* vert, const vector3& norm) const {
	// Compute the dot-product between the triangle normal and the light direction
	// When the normal is close to anti-parallel with the light direction, the triangle 
	//  will receive close to the maximum light level. When the angle between the two
	//  vectors approaches 90 degrees (and above), the triangle will receive little to no light.
	float dp = -dir.cosTheta(norm);
	return (dp > 0 ? brightness*dp : 0);
}

float pointLight::getIntensity(const Vertex* vert, const vector3& norm) const {
	vector3 displacement = (vert->getPosition() - pos);
	float dp = -displacement.cosTheta(norm);
	if(dp < 0) 
		return 0;
	return (brightness * dp / displacement.square());
}

float coneLight::getIntensity(const Vertex* vert, const vector3& norm) const {
	vector3 displacement = (vert->getPosition() - pos);
	float dp = -displacement.cosTheta(norm);
	if (dp < 0)
		return 0;
	dp *= radialIntensity(displacement.angle(dir));
	return (brightness * dp / displacement.square());
}

float coneLight::radialIntensity(const float& angle) const {
	if (angle <= openingAngle / 2)
		return 1.f;
	else if (angle <= openingAngle)
		return 2 * (1 - angle / openingAngle);
	return 0.f;
}