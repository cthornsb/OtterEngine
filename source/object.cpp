#include <iostream>
#include <float.h>

#include "object.hpp"

object::object() :
	built(false),
	reservedVertices(0),
	reservedPolygons(0),
	pos(),
	pos0(),
	rot(identityMatrix),
	center(),
	maxSize(),
	minSize(),
	vertices(),
	vertices0(),
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
	rot.setRotation(theta, phi, psi);
	
	// Update the rotation of all vertices
	transform(&rot);

	// Update rotation of child objects
	updateRotationOfChildren();
}

void object::move(const vector3 &offset){
	pos += offset;
	updatePositionOfChildren();
}

void object::setRotation(const float& theta, const float& phi, const float& psi){
	rot.setRotation(theta, phi, psi);

	// Reset all vertices to their original position
	resetVertices();
	
	// Update the rotation of all vertices
	transform(&rot);

	// Update rotation of child objects
	updateRotationOfChildren();
}

void object::setPosition(const vector3 &position){
	pos = position;
	updatePositionOfChildren();
}

void object::resetVertices(){
	vertices = vertices0;
	for (auto ch = children.begin(); ch != children.end(); ch++) {
		(*ch)->resetVertices();
	}
}

void object::resetPosition(){
	pos = pos0;
	updatePositionOfChildren();
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

void object::updatePositionOfChildren() {
	for (std::vector<object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
		(*ch)->updatePositionForParent(pos);
	}
}

void object::updatePositionForParent(const vector3& position) {
	pos = position + parentOffset;
}

void object::updateRotationOfChildren() {
	for (std::vector<object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
		(*ch)->updateRotationForParent(&rot);
		(*ch)->updatePositionForParent(pos); // Necessary because the offset position within the parent changed
	}
}

void object::updateRotationForParent(const matrix3* rotation) {
	rotation->transform(parentOffset);
	transform(rotation);
}

void object::reserve(const size_t& nVert, const size_t& nPoly/*=0*/) {
	reservedVertices = nVert;
	vertices.reserve(nVert);
	vertices0.reserve(nVert);
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
	for(std::vector<vector3>::iterator vert = vertices.begin(); vert != vertices.end(); vert++)
		mat->transform((*vert));
	
	// Update the normals of all polygons
	for(std::vector<triangle>::iterator tri = polys.begin(); tri != polys.end(); tri++)
		tri->update();
}

vector3* object::addVertex(const float&x, const float&y, const float&z){
	return addVertex(vector3(x, y, z));
}

vector3* object::addVertex(const vector3& vec) {
	if (vertices.size() > reservedVertices){
		std::cout << " Object: [warning] Not enough memory reserved for vertex vector, this may result in undefined behavior!" << std::endl;
	}
	vertices0.push_back(vec);
	vertices.push_back(vec);
	for (int i = 0; i < 3; i++) { // Update the size of the bounding box
		if (vec[i] > maxSize[i])
			maxSize[i] = vec[i];
		if (vec[i] < minSize[i])
			minSize[i] = vec[i];
	}
	return &vertices.back();
}

void object::addTriangle(const unsigned int& i0, const unsigned int& i1, const unsigned int& i2){
	if (polys.size() > reservedPolygons) {
		std::cout << " Object: [warning] Not enough memory reserved for polygon vector, this may result in undefined behavior!" << std::endl;
	}
	polys.push_back(triangle(vertices[i0], vertices[i1], vertices[i2]));
}

void object::addQuad(const unsigned int& i0, const unsigned int& i1, const unsigned int& i2, const unsigned int& i3) {
	// Eventually this will use a quad face, but for now we get two triangles
	addTriangle(i0, i1, i2);
	addTriangle(i2, i3, i0);
}

void object::addStaticTriangle(const unsigned int& i0, const unsigned int& i1, const unsigned int& i2) {
	polys.push_back(triangle(vertices0[i0], vertices0[i1], vertices0[i2]));
}

void object::addStaticQuad(const unsigned int& i0, const unsigned int& i1, const unsigned int& i2, const unsigned int& i3) {
	// Eventually this will use a quad face, but for now we get two triangles
	polys.push_back(triangle(vertices0[i0], vertices0[i1], vertices0[i2]));
	polys.push_back(triangle(vertices0[i2], vertices0[i3], vertices0[i0]));
}