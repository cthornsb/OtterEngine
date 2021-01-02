#include <iostream>
#include <float.h>
#include <map>

#include "Globals.hpp"
#include "object.hpp"
#include "camera.hpp"

object::object() :
	built(false),
	reservedVertices(0),
	reservedPolygons(0),
	pos(),
	pos0(),
	rot(identityMatrix),
	center(),
	pitchAngle(0, 0, 2 * pi),
	rollAngle(0, 0, 2 * pi),
	yawAngle(0, 0, 2 * pi),
	maxSize(),
	minSize(),
	vertices(),
	verticesToDraw(),
	polys(),
	children(),
	parentOffset(),
	parent(0x0)
{
	for (int i = 0; i < 3; i++) {
		maxSize[i] = -FLT_MAX;
		minSize[i] = +FLT_MAX;
	}
}

/** Object position constructor
  */
object::object(const vector3 & pos_) :
	object()
{
	pos = pos_;
	pos0 = pos_;
}

void object::rotate(const float& theta, const float& phi, const float& psi){
	// Update the rotation matrix
	rot.setRotation((pitchAngle += theta), (rollAngle += psi), (yawAngle += phi));

	// Update rotation of child objects
	updateRotation();
}

void object::move(const vector3 &offset){
	pos += offset;
	updatePosition();
}

void object::setRotation(const float& theta, const float& phi, const float& psi){
	// Update the rotation matrix
	rot.setRotation(theta, psi, phi);

	// Update rotation of child objects
	updateRotation();
}

void object::setPosition(const vector3 &position){
	pos = position;
	updatePosition();
}

void object::resetPosition(){
	pos = pos0;
	updatePosition();
}

void object::renderAllVertices(camera* cam) {
	for (std::vector<Vertex>::iterator vert = vertices.begin(); vert != vertices.end(); vert++) {
		cam->render(*vert);
	}
}

void object::addChild(object* child, const vector3& offset/*=zeroVector*/) {
	if (!child->built) { // Construct the child geometry (it not already done)
		child->build();
	}
	children.push_back(child);
	if (!child->setParent(this)) {
		std::cout << " Object: [warning] Child object already had a parent, this may result in undefined behavior!" << std::endl;
	}
	child->parentOffset = offset;
	child->updateRotationForParent(&rot);
	child->updatePositionForParent(pos);
}

void object::removeChild(object* child) {
	auto ch = std::find(children.begin(), children.end(), child);
	if (ch != children.end()) { // Remove the child and unlink it from this object
		(*ch)->parent = 0x0;
		(*ch)->parentOffset = zeroVector;
		children.erase(ch);
	}
	else {
		std::cout << " Object: [warning] Child object not found. Cannot remove" << std::endl;
	}
}

void object::build() {
	if (built)
		return;
	this->userBuild();
	// Finalize the object geometry
	// Compute the average normal vector at all vertices
	for (std::vector<triangle>::iterator tri = polys.begin(); tri != polys.end(); tri++) {
		for (std::vector<Vertex>::iterator vert = vertices.begin(); vert != vertices.end(); vert++) {
			if (tri->hasVertex(&(*vert)))
				vert->norm0 += tri->getNormal();
		}
	}
	// Normalize vertex normals
	for (std::vector<Vertex>::iterator vert = vertices.begin(); vert != vertices.end(); vert++) {
		vert->norm0.normInPlace();
	}
	verticesToDraw.reserve(vertices.size());
	built = true;
}

void object::setSizeX(const float& min_, const float& max_) {
	minSize[0] = min_;
	maxSize[0] = max_;
}

void object::setSizeY(const float& min_, const float& max_) {
	minSize[1] = min_;
	maxSize[1] = max_;
}

void object::setSizeZ(const float& min_, const float& max_) {
	minSize[2] = min_;
	maxSize[2] = max_;
}

bool object::setParent(object* obj) { 
	bool retval = (parent == 0x0);
	parent = obj;
	return retval;
}

void object::updatePosition() {
	for (std::vector<object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
		(*ch)->updatePositionForParent(pos);
	}
}

void object::updatePositionForParent(const vector3& position) {
	pos = position + parentOffset;
}

void object::updateRotation() {
	rot.transformInPlace(pos);
	for (std::vector<object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
		(*ch)->updateRotationForParent(&rot);
		(*ch)->updatePositionForParent(pos); // Necessary because the offset position within the parent changed
	}
}

void object::updateRotationForParent(const matrix3* rotation) {
	rotation->transformInPlace(parentOffset);
}

void object::reserve(const size_t& nVert, const size_t& nPoly/*=0*/) {
	reservedVertices = nVert;
	vertices.reserve(nVert);
	if (nPoly == 0) {
		reservedPolygons = nVert;
		polys.reserve(nVert);
	}
	else {
		reservedPolygons = nPoly;
		polys.reserve(nPoly);
	}
}

void object::transform(const matrix3* mat){
	// Transform all object vertices
	for(std::vector<Vertex>::iterator vert = vertices.begin(); vert != vertices.end(); vert++)
		vert->transform(mat);
	
	// Update the normals of all polygons
	for(std::vector<triangle>::iterator tri = polys.begin(); tri != polys.end(); tri++)
		tri->update();
}

Vertex* object::addVertex(const float&x, const float&y, const float&z){
	return addVertex(vector3(x, y, z));
}

Vertex* object::addVertex(const vector3& vec) {
	if (vertices.size() > reservedVertices){
		std::cout << " Object: [warning] Not enough memory reserved for vertex vector, this may result in undefined behavior!" << std::endl;
	}
	vertices.add(vec, this);
	for (int i = 0; i < 3; i++) { // Update the size of the bounding box
		if (vec[i] > maxSize[i])
			maxSize[i] = vec[i];
		if (vec[i] < minSize[i])
			minSize[i] = vec[i];
	}
	return vertices.back();
}

void object::addTriangle(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2){
	if (polys.size() > reservedPolygons) {
		std::cout << " Object: [warning] Not enough memory reserved for polygon vector, this may result in undefined behavior!" << std::endl;
	}
	polys.add(i0, i1, i2, &vertices, this);
}

void object::addQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3) {
	// Eventually this will use a quad face, but for now we get two triangles
	addTriangle(i0, i1, i2);
	addTriangle(i2, i3, i0);
}

void object::addStaticTriangle(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2) {
	polys.add(i0, i1, i2, &vertices, this);
}

void object::addStaticQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3) {
	// Eventually this will use a quad face, but for now we get two triangles
	polys.add(i0, i1, i2, &vertices, this);
	polys.add(i2, i3, i0, &vertices, this);
}