#include <cmath>

#include "Globals.hpp"
#include "Primitives.hpp"

void Primitives::Plane::userBuild() {
	// Reserve memory for geometry
	reserve(4, 2);

	// Add all vertices
	addVertex(dX, dY, 0);
	addVertex(dX, -dY, 0);
	addVertex(-dX, -dY, 0);
	addVertex(-dX, dY, 0);

	// Add the faces (using clockwise winding)
	addQuad(3, 2, 1, 0);
}

void Primitives::Cube::userBuild() {
	// Reserve memory for geometry
	reserve(8, 12);

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
	if (nVertices < 3) // Do not allow polygons with less than 3 sides
		nVertices = 3;

	// Reserve memory for geometry
	reserve(nVertices + 1, nVertices);

	// Add vertices circumscribed inside a circle of radius dR
	approximate(); 

	// Add all polygons (using clockwise winding)
	for (int i = 1; i < nVertices; i++) { // Add n-1 triangles
		addTriangle(0, i, i + 1);
	}
	// Add the final triangle
	addTriangle(0, nVertices, 1);
}

void Primitives::Circle::approximate(const float& zoffset/* = 0*/){
	// Add all vertices
	float angularStep = twoPi / nVertices;
	float currentAngle = 0;
	addVertex(0, 0, zoffset); // Add center vertex
	for (int i = 0; i < nVertices; i++) { // Add perimeter vertices
		// The negative x coordinate ensures proper winding when viewing down the Z axis
		addVertex(-dR*std::sin(currentAngle), dR*std::cos(currentAngle), zoffset);
		currentAngle += angularStep;
	}
}

void Primitives::Cylinder::userBuild() {
	if (nVertices < 3) // Do not allow cylinders with less than 3 sides
		nVertices = 3;

	// Reserve memory for geometry
	reserve(2 * (nVertices + 1), 4 * nVertices);

	//Build a circle of radius dR and offset to +dZ
	approximate(dZ);
		
	// Copy the vertices and offset to -dZ
	std::vector<Vertex> copy;
	vertices.copy(copy);
	for (auto vert = copy.begin(); vert != copy.end(); vert++)
		vert->offsetPosition(Vector3(0, 0, -dZ));

	// Must reverse the vertex order for correct winding 
	// because the circle faces the camera by default.
	std::reverse(vertices.begin() + 1, vertices.end());

	// Copy the new vertices to the vertex vectors
	vertices.append(copy);

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

	// Update the size along the z-axis because the vertices were moved manually
	setSizeZ(-dZ, dZ);
}

void Primitives::Cone::userBuild() {
	if (nVertices < 3) // Do not allow cones with less than 3 sides
		nVertices = 3;

	// Reserve memory for geometry
	reserve(nVertices + 2, 2 * nVertices);

	//Build a circle of radius dR and offset to -dZ
	approximate(-dZ);

	// Add the peak vertex
	vertices.add(0, 0, dZ, this);

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

	// Update the size along the z-axis because the vertices were moved manually
	setSizeZ(-dZ, dZ);
}