#include <iostream>

#include "triangle.hpp"
#include "object.hpp"

/** Position / normal pointer constructor
  */
triangle::triangle(Vertex* p0_, Vertex* p1_, Vertex* p2_, const object* obj) :
	plane(p0_->pos0, p1_->pos0, p2_->pos0),
	p0(p0_),
	p1(p1_),
	p2(p2_),
	offset(obj->getConstPositionPointer()),
	rotation(obj->getConstRotationMatrix()),
	parent(obj)
{
}

Vector3 triangle::getCenterPoint() const {
	return rotation->transform(p + *offset);
}

Vector3 triangle::getNormal() const {
	return rotation->transform(norm);
}

void triangle::update(){
	// Update the plane of the triangle
	Vector3 p1p0 = p1->pos0 - p0->pos0; // Vector from p0 to p1
	Vector3 p2p0 = p2->pos0 - p0->pos0; // Vector from p0 to p2
	
	// Compute the normal to the plane
	norm = p2p0.cross(p1p0).normalize();
	p = (p0->pos0 + p1->pos0 + p2->pos0) * (1 / 3.0f);
}

void triangle::dump() const {
	std::cout << " TRIANGLE::\n";
	//p0->dump();
	//p1->dump();
	//p2->dump();
}
