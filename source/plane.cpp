#include <iostream>

#include "plane.hpp"

plane::plane(const Vector3 &p0_, const Vector3 &p1_, const Vector3 &p2_){
	Vector3 p1p0 = p1_-p0_; // Vector from p0 to p1
	Vector3 p2p0 = p2_-p0_; // Vector from p0 to p2
	
	// Compute the normal to the plane
	norm = p2p0.cross(p1p0).normalize();
	p = (p0_ + p1_ + p2_)*(1/3.0f);
}

bool plane::intersects(const ray& r) const {
	float dummy;
	return intersects(r, dummy);
}

bool plane::intersects(const ray& r, float& t) const {
	float denom = r.dir * norm;
	if(denom == 0)
		return false;
	float numer = (p - r.pos) * norm;
	t = numer/denom;
	return (numer < 0 && denom < 0);
}

