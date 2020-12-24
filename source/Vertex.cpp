#include "Vertex.hpp"
#include "object.hpp"
#include "matrix3.hpp"

Vertex::Vertex(const vector3& position, const object* obj) :
	pX(0),
	pY(0),
	sX(0),
	sY(0),
	zDepth(0),
	pos0(position),
	norm0(),
	offset(obj->getConstPositionPointer()),
	rotation(obj->getConstRotationMatrix()),
	parent(obj)
{
}

vector3 Vertex::getPosition() const { 
	return rotation->transform(pos0 + *offset); 
}

vector3 Vertex::getNormal() const {
	return rotation->transform(norm0);
}

vector3 Vertex::transform(const matrix3* mat) {
	return mat->transform(pos0);
}
