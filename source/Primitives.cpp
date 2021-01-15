#include <cmath>
#include <iostream> // TEMP

#include "Globals.hpp"
#include "Primitives.hpp"

void Primitives::Plane::userBuild() {
	// Reserve memory for geometry
	reserve(4, 2);

	// Texture coordinates
	std::vector<Vector2> uvCoords = {
		Vector2(1.f, 0.f), // Top right
		Vector2(1.f, 1.f), // Bottom right
		Vector2(0.f, 1.f), // Bottom left
		Vector2(0.f, 0.f)  // Top left
	};

	// Add all vertices
	addVertex(Vector3(dX, dY, 0.f), uvCoords[0]); // Top right
	addVertex(Vector3(dX, -dY, 0.f), uvCoords[1]); // Bottom right
	addVertex(Vector3(-dX, -dY, 0.f), uvCoords[2]); // Bottom left
	addVertex(Vector3(-dX, dY, 0.f), uvCoords[3]); // Top left

	// Add the faces (using clockwise winding)
	addQuad(0, 1, 2, 3);
}

void Primitives::Cube::userBuild() {
	// Reserve memory for geometry
	reserve(8, 12);

	// Texture coordinates
	std::vector<Vector2> uvCoords = {
		Vector2(1.f, 0.f), // Bottom right
		Vector2(0.f, 0.f), // Bottom left
		Vector2(0.f, 1.f), // Top left
		Vector2(1.f, 1.f)  // Top right
	};

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
	addQuad(0, 1, 2, 3, uvCoords[2], uvCoords[3], uvCoords[0], uvCoords[1]); // +Z
	addQuad(5, 4, 7, 6, uvCoords[0], uvCoords[1], uvCoords[2], uvCoords[3]); // -Z
	addQuad(1, 0, 4, 5, uvCoords[0], uvCoords[1], uvCoords[2], uvCoords[3]); // +X	
	addQuad(7, 3, 2, 6, uvCoords[3], uvCoords[0], uvCoords[1], uvCoords[2]); // -X
	addQuad(7, 4, 0, 3, uvCoords[1], uvCoords[2], uvCoords[3], uvCoords[0]); // +Y
	addQuad(6, 2, 1, 5, uvCoords[2], uvCoords[3], uvCoords[0], uvCoords[1]); // -Y
}

void Primitives::Circle::userBuild() {
	if (nVertices < 3) // Do not allow polygons with less than 3 sides
		nVertices = 3;

	// Reserve memory for geometry
	reserve(nVertices + 1, nVertices);

	// Add vertices circumscribed inside a circle of radius dR
	std::vector<Vector2> uvCoords = { Vector2(0.f, 0.f) };
	approximate(uvCoords); 

	// Add all vertices
	Vector2 center(0.5f, 0.5f);
	for (int i = 0; i <= nVertices; i++) { // Add perimeter vertices
		addVertex(Vector3(2 * dR * uvCoords[i][0], -2 * dR * uvCoords[i][1], 0.f), uvCoords[i] + center);
	}

	// Add all polygons (using clockwise winding)
	for (int i = 1; i < nVertices; i++) { // Add n-1 triangles
		addTriangle(0, i + 1, i);
	}

	// Add the final triangle
	addTriangle(0, 1, nVertices);
}

void Primitives::Circle::approximate(std::vector<Vector2>& coords){
	// Add all vertices
	float angularStep = twoPi / nVertices;
	float currentAngle = 0;
	//addVertex(0, 0, zoffset); // Add center vertex
	for (int i = 0; i < nVertices; i++) { // Add perimeter vertices
		// The negative x coordinate ensures proper winding when viewing down the Z axis
		coords.push_back(Vector2(std::sin(currentAngle) / 2.f, std::cos(currentAngle) / 2.f));
		currentAngle += angularStep;
	}
}

void Primitives::Cylinder::userBuild() {
	if (nVertices < 3) // Do not allow cylinders with less than 3 sides
		nVertices = 3;

	// Reserve memory for geometry
	reserve(2 * (nVertices + 1), 4 * nVertices);

	//Build a circle of radius dR and offset to +dZ
	std::vector<Vector2> uvCoords = { Vector2(0.f, 0.f) };
	approximate(uvCoords);

	// Add all vertices
	Vector2 center(0.5f, 0.5f);
	for (int i = 0; i <= nVertices; i++) { // Add perimeter vertices
		addVertex(Vector3(2 * dR * uvCoords[i][0], -2 * dR * uvCoords[i][1], -dZ), uvCoords[i] + center);
		addVertex(Vector3(2 * dR * uvCoords[i][0], -2 * dR * uvCoords[i][1], dZ), uvCoords[i] + center);
	}

	// Construct geometry
	for (int i = 1; i < nVertices; i++) { // Add n-1 triangles
		// Endcaps
		addTriangle(0, 2 * i, 2 * (i + 1)); // -Z
		addTriangle(1, 2 * (i + 1) + 1, 2 * i + 1); // +Z (reversed winding)

		// Sides
		addTriangle(2 * (i + 1), 2 * i, 2 * (i + 1) + 1);
		addTriangle(2 * i + 1, 2 * (i + 1) + 1, 2 * i);
	}
	
	// Add the final triangles
	addTriangle(0, 2 * nVertices, 2);
	addTriangle(1, 3, 2 * nVertices + 1);
	
	// Add final side
	addTriangle(2, 2 * nVertices, 3);
	addTriangle(2 * nVertices + 1, 3, 2 * nVertices);
}

void Primitives::Cone::userBuild() {
	if (nVertices < 3) // Do not allow cones with less than 3 sides
		nVertices = 3;

	// Reserve memory for geometry
	reserve(nVertices + 2, 2 * nVertices);

	//Build a circle of radius dR and offset to -dZ
	std::vector<Vector2> uvCoords = { Vector2(0.f, 0.f) };
	approximate(uvCoords);

	// Add all base vertices
	Vector2 center(0.5f, 0.5f);
	for (int i = 0; i <= nVertices; i++) { // Add perimeter vertices
		addVertex(Vector3(2 * dR * uvCoords[i][0], -2 * dR * uvCoords[i][1], 0.f), uvCoords[i] + center);
	}

	// Add the peak vertex
	addVertex(Vector3(0.f, 0.f, dZ), center);

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