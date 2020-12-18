#include <iostream>

#include "object.hpp"

void object::rotate(const float& theta, const float& phi, const float& psi){
	rot.setRotation(theta, phi, psi);
	
	// Update the rotation of all vertices
	transform();
}

void object::move(const vector3 &offset){
	pos += offset;
}

void object::setRotation(const float& theta, const float& phi, const float& psi){
	rot.setRotation(theta, phi, psi);

	// Reset all vertices to their original position
	resetVertices();
	
	// Update the rotation of all vertices
	transform();
}

void object::setPosition(const vector3 &position){
	pos = position;
}

void object::resetVertices(){
	vertices = vertices0;
}

void object::resetPosition(){
	pos = pos0;
}

void object::build() {
	if (built)
		return;
	this->userBuild();
	built = true;
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

void object::transform(){
	// Transform all object vertices
	for(std::vector<vector3>::iterator vert = vertices.begin(); vert != vertices.end(); vert++)
		rot.transform((*vert));
	
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