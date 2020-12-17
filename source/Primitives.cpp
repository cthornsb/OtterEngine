#include <iostream>
#include <cmath>

#include "Globals.hpp"
#include "Primitives.hpp"

void Primitives::Plane::userBuild() {
	// Add all vertices
	addVertex(dX, dY, 0);
	addVertex(dX, -dY, 0);
	addVertex(-dX, -dY, 0);
	addVertex(-dX, dY, 0);

	// Add the faces (using clockwise winding)
	addQuad(3, 2, 1, 0);
}

void Primitives::Cube::userBuild() {
	// Add all vertices
	addVertex(dX, dY, dZ);
	addVertex(dX, -dY, dZ);
	addVertex(-dX, -dY, dZ);
	addVertex(-dX, dY, dZ);
	addVertex(dX, dY, -dZ);
	addVertex(dX, -dY, -dZ);
	addVertex(-dX, -dY, -dZ);
	addVertex(-dX, dY, -dZ);

	// Add the faces (using clockwise winding)
	addQuad(0, 1, 2, 3);
	addQuad(1, 0, 4, 5);
	addQuad(5, 4, 7, 6);
	addQuad(7, 3, 2, 6);
	addQuad(7, 4, 0, 3);
	addQuad(6, 2, 1, 5);
}

void Primitives::Circle::userBuild() {
	approximate(); // Add vertices circumscribed inside a circle of radius dR

	// Add all polygons (using clockwise winding)
	for (int i = 1; i < nVertices; i++) { // Add n-1 triangles
		addTriangle(0, i, i + 1);
	}
	// Add the final triangle
	addTriangle(0, nVertices, 1);
}

void Primitives::Circle::approximate(const double& zoffset/* = 0*/){
	if (nVertices < 3) // Do not allow polygons with less than 3 sides
		nVertices = 3;

	// Add all vertices
	//double sign = (zoffset >= 0 ? 1 : -1);
	double angularStep = twoPi / nVertices;
	double currentAngle = 0;
	addVertex(0, 0, zoffset); // Add center vertex
	for (int i = 0; i < nVertices; i++) { // Add perimeter vertices
		// The negative x coordinate ensures proper winding when viewing down the Z axis
		addVertex(-dR*std::sin(currentAngle), dR*std::cos(currentAngle), zoffset);
		currentAngle += angularStep;
	}
}

void Primitives::Cylinder::userBuild() {
	//Build a circle of radius dR and offset to +dZ
	approximate(dZ);
		
	// Copy the vertices and offset to -dZ
	std::vector<vector3> copy = vertices0;
	for (auto vert = copy.begin(); vert != copy.end(); vert++)
		vert->z = -dZ;

	// Must reverse the vertex order for correct winding 
	// because the circle faces the camera by default.
	std::reverse(vertices.begin() + 1, vertices.end());
	std::reverse(vertices0.begin() + 1, vertices0.end());

	// Copy the new vertices to the vertex vectors
	vertices.insert(vertices.begin(), copy.begin(), copy.end());
	vertices0.insert(vertices0.begin(), copy.begin(), copy.end());

	// Construct negative endcap
	//approximate(-dZ);
	for (int i = 1; i < nVertices; i++) { // Add n-1 triangles
		addTriangle(0, i, i + 1);
	}
	// Add the final triangle
	addTriangle(0, nVertices, 1);

	// Construct positive endcap 
	for (int i = 1; i < nVertices; i++) { // Add n-1 triangles
		addTriangle(nVertices + 1, nVertices + 1 + i, nVertices + 2 + i);
	}
	// Add the final triangle
	addTriangle(nVertices + 1, 2*nVertices + 1, nVertices + 2);

	// Add sides of cylinder
	addQuad(1, nVertices, nVertices + 2, 2 * nVertices + 1);
	for (int i = 0; i <= nVertices-2; i++) {
		addQuad(i+2, i+1, 2*nVertices+1-i, 2*nVertices-i);
	}
}

void Primitives::Cone::userBuild() {
	//Build a circle of radius dR and offset to -dZ
	approximate(-dZ);

	// Add the peak vertex
	vertices.push_back(vector3(0, 0, dZ));
	vertices0.push_back(vector3(0, 0, dZ));

	// Fill the base
	for (int i = 1; i < nVertices; i++) { // Add n-1 triangles
		addTriangle(0, i, i + 1);
	}
	// Add the final triangle
	addTriangle(0, nVertices, 1);

	// Add sides of cone
	for (int i = 1; i < nVertices; i++) {
		addTriangle(i+1, i, nVertices+1);
	}
	// Add the final triangle
	addTriangle(1, nVertices, nVertices+1);
}