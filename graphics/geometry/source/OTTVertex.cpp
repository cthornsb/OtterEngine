#include "OTTVertex.hpp"
#include "OTTObject.hpp"

ott::Vertex::Vertex(const Vector3& position, const Object* obj) :
#if defined(SOFTWARE_RENDERER)
	pX(0),
	pY(0),
	sX(0),
	sY(0),
	zDepth(0),
#endif // #if defined(SOFTWARE_RENDERER)
	pos0(position),
	norm0(),
	color(1.f, 1.f, 1.f),
	texCoords(0.f, 0.f),
	offset(obj->ConstPositionPointer()),
	rotation(obj->ConstRotationMatrix())
{
}

ott::Vertex::Vertex(const Vector3& position, const Vector3& normal, const Object* obj) :
#if defined(SOFTWARE_RENDERER)
	pX(0),
	pY(0),
	sX(0),
	sY(0),
	zDepth(0),
#endif // #if defined(SOFTWARE_RENDERER)
	pos0(position),
	norm0(normal),
	color(1.f, 1.f, 1.f),
	texCoords(0.f, 0.f),
	offset(obj->ConstPositionPointer()),
	rotation(obj->ConstRotationMatrix())
{
}

ott::Vector3 ott::Vertex::Position() const {
	if (offset == nullptr) {
		return pos0;
	}
	return rotation->Transform(pos0 + *offset); 
}

ott::Vector3 ott::Vertex::Normal() const {
	if (rotation == nullptr) {
		return norm0;
	}
	return rotation->Transform(norm0);
}

ott::Vector3 ott::Vertex::Transform(const Matrix3* mat) {
	return mat->Transform(pos0);
}
