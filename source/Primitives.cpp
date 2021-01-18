#include <cmath>
#include <iostream> // TEMP

#include "Globals.hpp"
#include "Primitives.hpp"

void approximateCircle(std::vector<Vector2>& coords, const int& nVertices, const float& dR=0.5f) {
	// Add all vertices
	float angularStep = twoPi / nVertices;
	float currentAngle = 0;
	for (int i = 0; i < nVertices; i++) { // Add perimeter vertices
		// The negative x coordinate ensures proper winding when viewing down the Z axis
		coords.push_back(Vector2(std::sin(currentAngle), std::cos(currentAngle)) * dR);
		currentAngle += angularStep;
	}
}

Vector3 Primitives::defaultVertexPosition(const float& px, const float& py, const float& pz) {
	return Vector3(px, py, pz);
}

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
	approximateCircle(uvCoords, nVertices);

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

void Primitives::Cylinder::userBuild() {
	if (nVertices < 3) // Do not allow cylinders with less than 3 sides
		nVertices = 3;

	// Reserve memory for geometry
	reserve(2 * (nVertices + 1), 4 * nVertices);

	//Build a circle of radius dR and offset to +dZ
	std::vector<Vector2> uvCoords = { Vector2(0.f, 0.f) };
	approximateCircle(uvCoords, nVertices);

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
	approximateCircle(uvCoords, nVertices);

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
}

void Primitives::Sphere::userBuild() {
	if (nDivLatitude < 2)
		nDivLatitude = 2;
	if (nDivLongitude < 3)
		nDivLongitude = 3;

	// Add two more rings to better approximate texture map at poles
	nDivLatitude += 2;

	// Reserve memory for geometry
	// Top and bottom caps: nDivLongitude faces each
	// Every latitude ring: nDivLongitude * 2 faces each
	// Total number of vertices: 2 + nDivLongitude * nDivLatitude
	// Total number of faces: 2 * nDivLongitude * nDivLatitude
	reserve(2 + nDivLongitude * nDivLatitude, 2 * nDivLongitude * nDivLatitude);

	// Add all vertices
	std::vector<Vector2> uvCoords;
	Vector2 uv = computeUV(Vector3(0.f, dR, 0.f));
	addVertex(Vector3(0.f, dR, 0.f), uv); // North pole
	uvCoords.push_back(uv);

	float zStep = 2 * dR / (nDivLatitude + 1);
	float dY = dR; // Start at the top of sphere

	// North pole endcap
	addLatitudePoints(dR, dR - zStep / 2, uvCoords);
	float maximumV = (1.f + uvCoords.back()[1]) / 2;

	for (int lat = 0; lat < nDivLatitude - 2; lat++) {
		addLatitudePoints(dR, (dY -= zStep), uvCoords);
	}

	// South pole endcap
	addLatitudePoints(dR, -dR + zStep / 2, uvCoords);
	float minimumV = uvCoords.back()[1] / 2;

	uv = computeUV(Vector3(0.f, -dR, 0.f));
	addVertex(Vector3(0.f, -dR, 0.f), uv); // South pole
	uvCoords.push_back(uv);

	// Add "north pole" faces
	for (int i = 1; i < nDivLongitude; i++) {
		addTriangle(
			0, 
			i + 1,
			i, 
			Vector2((uvCoords[i + 1][0] + uvCoords[i][0]) / 2, maximumV),
			uvCoords[i + 1],
			uvCoords[i]
		);
	}
	addTriangle(
		0,
		1,
		nDivLongitude,
		Vector2((1.f + uvCoords[nDivLongitude][0]) / 2, maximumV),
		Vector2(1.f, uvCoords[1][1]),
		uvCoords[nDivLongitude]
	);

	// Add quads for all rings
	for (int i = 0; i < nDivLatitude - 1; i++) {
		int topRingIndex = 1 + nDivLongitude * i;
		int bottomRingIndex = 1 + nDivLongitude * (i + 1);
		for (int j = 0; j < nDivLongitude - 1; j++) {
			addQuad(topRingIndex + j, topRingIndex + j + 1, bottomRingIndex + j + 1, bottomRingIndex + j);
		}
		addQuad(
			topRingIndex + nDivLongitude - 1, 
			topRingIndex, 
			bottomRingIndex, 
			bottomRingIndex + nDivLongitude - 1,
			uvCoords[topRingIndex + nDivLongitude - 1],
			Vector2(1.f, uvCoords[topRingIndex][1]),
			Vector2(1.f, uvCoords[bottomRingIndex][1]),
			uvCoords[bottomRingIndex + nDivLongitude - 1]
		);
	}

	// Add "south pole" faces
	int topRingIndex = 1 + nDivLongitude * (nDivLatitude - 1);
	for (int i = 0; i < nDivLongitude - 1; i++) {
		addTriangle(
			vertices.size() - 1, 
			topRingIndex + i, 
			topRingIndex + i + 1,
			Vector2((uvCoords[topRingIndex + i][0] + uvCoords[topRingIndex + i + 1][0]) / 2, minimumV),
			uvCoords[topRingIndex + i],
			uvCoords[topRingIndex + i + 1]
		);
	}
	addTriangle(
		vertices.size() - 1,
		topRingIndex + nDivLongitude - 1,
		topRingIndex,
		Vector2((1.f + uvCoords[topRingIndex + nDivLongitude][0]) / 2, minimumV),
		uvCoords[topRingIndex + nDivLongitude - 1],
		Vector2((1.f + uvCoords[topRingIndex][0]) / 2, minimumV)
	);
}

Vector2 Primitives::Sphere::computeUV(const Vector3& surfPos) const {
	Vector3 uD = (surfPos * -1).normalize();
	return Vector2(
		0.5f + std::atan2(uD[0], uD[2]) / (2 * pi),
		0.5f - std::asin(uD[1]) / pi
	);
}

void Primitives::Sphere::addLatitudePoints(const float& dR, const float& dY, std::vector<Vector2>& uvCoords) {
	std::vector<Vector2> coords;
	float r = std::sqrtf(dR * dR - dY * dY);
	approximateCircle(coords, nDivLongitude, r);
	for (auto point = coords.cbegin(); point != coords.cend(); point++) {
		Vector3 vpos((*point)[0], dY, (*point)[1]);
		Vector2 uv = computeUV(vpos);
		addVertex(vpos, uv);
		uvCoords.push_back(uv);
	}
}

void Primitives::Mesh::userBuild() {
	if (nDivX < 1)
		nDivX = 1;
	if (nDivZ < 1)
		nDivZ = 1;

	// Reserve memory for geometry
	reserve((nDivX + 1) * (nDivZ + 1), nDivX * nDivZ * 2.f);

	// Add all vertices
	float xStep = dX / nDivX;
	float zStep = dZ / nDivZ;
	float xPos = -dX / 2.f;
	float zPos = -dZ / 2.f;
	for (int i = 0; i <= nDivZ; i++) {
		xPos = -dX / 2.f;
		for (int j = 0; j <= nDivX; j++) {
			Vector3 vertPos = (*pfunc)(xPos, 0.f, zPos);
			addVertex(vertPos, Vector2((vertPos[0] * 2 / dX + 1) / 2, (-vertPos[2] * 2 / dZ + 1) / 2));
			xPos += xStep;
		}
		zPos += zStep;
	}

	// Add all quads
	for (int i = 0; i < nDivZ; i++) {
		for (int j = 0; j < nDivX; j++) {
			addQuad(i * (nDivX + 1) + j, i * (nDivX + 1) + j + 1, (i + 1) * (nDivX + 1) + j + 1, (i + 1) * (nDivX + 1) + j);
		}
	}
}