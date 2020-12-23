#include <iostream>

#include "triangle.hpp"

vector3 triangle::getCenterPoint() const {
	return (p + *offset);
}

void triangle::update(){
	// Update the plane of the triangle
	vector3 p1p0 = p1->pos - p0->pos; // Vector from p0 to p1
	vector3 p2p0 = p2->pos - p0->pos; // Vector from p0 to p2
	
	// Compute the normal to the plane
	norm = p2p0.cross(p1p0).normalize();
	p = (p0->pos + p1->pos + p2->pos) * (1 / 3.0f);
}

void triangle::dump() const {
	std::cout << " TRIANGLE::\n";
	//p0->dump();
	//p1->dump();
	//p2->dump();
}
