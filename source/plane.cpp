#include <iostream>

#include "plane.hpp"

plane::plane(const vector3 &p0_, const vector3 &p1_, const vector3 &p2_){
	vector3 p1p0 = p1_-p0_; // Vector from p0 to p1
	vector3 p2p0 = p2_-p0_; // Vector from p0 to p2
	
	// Compute the normal to the plane
	norm = p2p0.cross(p1p0).normalize();
	p = (p0_ + p1_ + p2_)*(1/3.0f);
}

bool plane::intersects(const ray& r) const {
	float dummy;
	return intersects(r, dummy);
}

bool plane::intersects(const ray& r, float& t) const {
	float numer = (p - r.pos) * norm;
	float denom = r.dir * norm;
	if(denom == 0)
		return false;
	t = numer/denom;
	return (numer < 0 && denom < 0);
}

void plane::dump() const {
	std::cout << " PLANE::\n";
	p.dump();
	norm.dump();
}
