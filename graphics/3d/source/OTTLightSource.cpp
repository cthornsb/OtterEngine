#include "OTTLightSource.hpp"

#include <graphics/geometry/OTTVertex.hpp>

#include <cmath>

float ott::LightSource::Intensity(const Vertex* vert, const Vector3& norm) const {
	// Compute the dot-product between the triangle normal and the light direction
	// When the normal is close to anti-parallel with the light direction, the triangle 
	//  will receive close to the maximum light level. When the angle between the two
	//  vectors approaches 90 degrees (and above), the triangle will receive little to no light.
	float dp = -ray.dir.CosTheta(norm);
	return (dp > 0 ? brightness*dp : 0);
}

float ott::PointLight::Intensity(const Vertex* vert, const Vector3& norm) const {
	Vector3 displacement = (vert->Position() - ray.pos);
	float dp = -displacement.CosTheta(norm);
	if (dp < 0) {
		return 0;
	}
	return (brightness * dp / displacement.Square());
}

float ott::ConeLight::Intensity(const Vertex* vert, const Vector3& norm) const {
	Vector3 displacement = (vert->Position() - ray.pos);
	float dp = -displacement.CosTheta(norm);
	if (dp < 0) {
		return 0;
	}
	dp *= this->RadialIntensity(displacement.Angle(ray.dir));
	return (brightness * dp / displacement.Square());
}

float ott::ConeLight::RadialIntensity(const float& angle) const {
	if (angle <= openingAngle / 2) {
		return 1.f;
	}
	else if (angle <= openingAngle) {
		return 2 * (1 - angle / openingAngle);
	}
	return 0.f;
}
