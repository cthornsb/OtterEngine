#include <iostream>

#include "triangle.hpp"

void triangle::update(){
	// Update the plane of the triangle
	vector3 p1p0 = (*p1) - (*p0); // Vector from p0 to p1
	vector3 p2p0 = (*p2) - (*p0); // Vector from p0 to p2
	
	// Compute the normal to the plane
	norm = p2p0.cross(p1p0).normalize();
	p = ((*p0) + (*p1) + (*p2))*(1/3.0);
}

void triangle::dump() const {
	std::cout << " TRIANGLE::\n";
	p0->dump();
	p1->dump();
	p2->dump();
}
