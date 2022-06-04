#include "OTTPolygon.hpp"
#include "OTTVertex.hpp"

ott::Polygon::Polygon(const Vertex* p0_, const Vertex* p1_, const Vertex* p2_) :
	Plane(p0_->pos0, p1_->pos0, p2_->pos0),
	p0(nullptr),
	p1(nullptr),
	p2(nullptr),
	offset(nullptr),
	rotation(nullptr),
	parent(nullptr)
{
}

/** Position / normal pointer constructor
  */
ott::Polygon::Polygon(const Vertex* p0_, const Vertex* p1_, const Vertex* p2_, const Object* obj) :
	Plane(p0_->pos0, p1_->pos0, p2_->pos0),
	p0(nullptr),
	p1(nullptr),
	p2(nullptr),
	offset(nullptr),
	rotation(nullptr),
	parent(nullptr)
{
}

bool ott::Polygon::ContainsVertex(Vertex* ptr) const {

	return (ptr == p0 || ptr == p1 || ptr == p2);
}

void ott::Polygon::OffsetCenter(Vector3 displacement) {

	// todo
}

void ott::Polygon::Update() {

	// Update the plane of the triangle
	Vector3 p1p0 = p1->pos0 - p0->pos0; // Vector from p0 to p1
	Vector3 p2p0 = p2->pos0 - p0->pos0; // Vector from p0 to p2
	
	// Compute the normal to the plane
	this->norm = p2p0.Cross(p1p0).Normalize();
	this->pos = (p0->pos0 + p1->pos0 + p2->pos0) * (1 / 3.0f);
}
