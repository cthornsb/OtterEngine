#include <iostream>

#include "plane.hpp"

plane::plane(const vector3 &p0_, const vector3 &p1_, const vector3 &p2_){
	vector3 p1p0 = p1_-p0_; // Vector from p0 to p1
	vector3 p2p0 = p2_-p0_; // Vector from p0 to p2
	
	// Compute the normal to the plane
	norm = p2p0.cross(p1p0).normalize();
	p = (p0_ + p1_ + p2_)*(1/3.0);
}

bool plane::intersects(const ray &r) const {
	double dummy;
	return intersects(r, dummy);
}

bool plane::intersects(const ray &r, double &t) const {
	t = ((p * norm) - (r.pos * norm)) / (r.dir * norm);
	return (t > 0);
}

void plane::dump() const {
	std::cout << " PLANE::\n";
	p.dump();
	norm.dump();
}
