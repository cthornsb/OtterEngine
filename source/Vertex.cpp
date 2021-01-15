#include "Vertex.hpp"
#include "object.hpp"
#include "Matrix.hpp"

Vertex::Vertex(const Vector3& position, const object* obj) :
	pX(0),
	pY(0),
	sX(0),
	sY(0),
	zDepth(0),
	pos0(position),
	norm0(),
	color(1.f, 1.f, 1.f),
	texCoords(0.f, 0.f),
	offset(obj->getConstPositionPointer()),
	rotation(obj->getConstRotationMatrix()),
	parent(obj)
{
}

Vector3 Vertex::getPosition() const { 
	return rotation->transform(pos0 + *offset); 
}

Vector3 Vertex::getNormal() const {
	return rotation->transform(norm0);
}

Vector3 Vertex::transform(const Matrix3* mat) {
	return mat->transform(pos0);
}
