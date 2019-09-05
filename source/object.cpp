#include "object.hpp"

void object::rotate(const double &theta, const double &phi, const double &psi){
	rot.setRotation(theta, phi, psi);
	
	// Update the rotation of all vertices
	transform();
}

void object::setRotation(const double &theta, const double &phi, const double &psi){
	rot.setRotation(theta, phi, psi);

	// Reset all vertices to their original position
	resetVertices();
	
	// Update the rotation of all vertices
	transform();
}

void object::resetVertices(){
	vertices = vertices0;
}

void object::resetPosition(){
	pos = pos0;
}

void object::transform(){
	// Transform all object vertices
	for(std::vector<vector3>::iterator vert = vertices.begin(); vert != vertices.end(); vert++)
		rot.transform((*vert));
	
	// Update the normals of all polygons
	for(std::vector<triangle>::iterator tri = polys.begin(); tri != polys.end(); tri++)
		tri->update();
}

void object::addVertex(const double &x, const double &y, const double &z){ 
	vertices0.push_back(vector3(x, y, z)); 
	vertices.push_back(vertices0.back());
}

void object::addPolygon(const size_t &i0, const size_t &i1, const size_t &i2){
	polys.push_back(triangle(vertices[i0], vertices[i1], vertices[i2]));
}
