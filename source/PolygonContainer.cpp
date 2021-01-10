#include <iostream>
#include <GL/glew.h>

#include "PolygonContainer.hpp"
#include "triangle.hpp"
#include "object.hpp"

PolygonContainer::~PolygonContainer() {
	// Delete VBOs
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &indexVBO);
}

void PolygonContainer::reserve(const size_t& N) {
	rawVertices.reserve(9 * N);
	rawNormals.reserve(9 * N);
	indicies.reserve(3 * N);
	polys.reserve(N);
}

void PolygonContainer::add(const triangle& tri, VertexContainer* vertices) {
	polys.push_back(tri);
	addVertex(tri.p2);
	addVertex(tri.p1);
	addVertex(tri.p0);
}

void PolygonContainer::add(Vertex* v0, Vertex* v1, Vertex* v2, VertexContainer* vertices, const object* obj) {
	polys.push_back(triangle(v0, v1, v2, obj));
	addVertex(v2);
	addVertex(v1);
	addVertex(v0);
}

void PolygonContainer::add(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, VertexContainer* vertices, const object* obj) {
	polys.push_back(triangle((*vertices)[i0], (*vertices)[i1], (*vertices)[i2], obj));
	addVertex((*vertices)[i2]);
	addVertex((*vertices)[i1]);
	addVertex((*vertices)[i0]);
}

void PolygonContainer::finalize() {
	// Setup buffer objects
	setupVBOs();

	// Data has been sent to GPU, delete raw data in conventional memory
	rawVertices.clear();
	rawNormals.clear();
	rawTextureCoords.clear();
	rawColorData.clear();
	indicies.clear();
}

void PolygonContainer::addVertex(Vertex* vert) {
	Vector3 pos  = vert->getInitialPosition();
	Vector3 norm = back()->getInitialNormal();
	Vector2 text = vert->getTextureCoordinates();
	ColorRGB color = vert->getColor();

	indicies.push_back((unsigned short)(rawVertices.size() / 3));

	rawVertices.push_back(pos[0]);
	rawVertices.push_back(pos[1]);
	rawVertices.push_back(pos[2]);

	rawNormals.push_back(norm[0]);
	rawNormals.push_back(norm[1]);
	rawNormals.push_back(norm[2]);

	rawColorData.push_back(color.r);
	rawColorData.push_back(color.g);
	rawColorData.push_back(color.b);

	rawTextureCoords.push_back(text[0]);
	rawTextureCoords.push_back(text[1]);
}

void PolygonContainer::setupVBOs() {
	// Generate vertex VBO
	glCreateBuffers(1, &vertexVBO);
	glCreateBuffers(1, &indexVBO);

	std::cout << " [debug] vertexVBO=" << vertexVBO << ", indexVBO=" << indexVBO << std::endl;

	sizeOfVertexData = rawVertices.size() * sizeof(float);
	sizeOfNormalData = rawNormals.size() * sizeof(float);
	sizeOfColorData = rawColorData.size() * sizeof(float);
	sizeOfTextureData = rawTextureCoords.size() * sizeof(float);
		
	size_t totalSize = sizeOfVertexData + sizeOfNormalData + sizeOfColorData + sizeOfTextureData;
	offsetVertex = 0;
	offsetNormal = sizeOfVertexData;
	offsetColor = sizeOfVertexData + sizeOfNormalData;
	offsetTexture = sizeOfVertexData + sizeOfNormalData + sizeOfColorData;
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glNamedBufferData(vertexVBO, totalSize, 0x0, GL_STATIC_DRAW); // Reserve vertex buffer memory
	glNamedBufferSubData(vertexVBO, 0, sizeOfVertexData, rawVertices.data()); // Copy vertices
	glNamedBufferSubData(vertexVBO, offsetNormal, sizeOfNormalData, rawNormals.data()); // Copy normals after vertices
	glNamedBufferSubData(vertexVBO, offsetColor, sizeOfColorData, rawColorData.data()); // Copy colors after norms
	glNamedBufferSubData(vertexVBO, offsetTexture, sizeOfTextureData, rawTextureCoords.data()); // Copy texture coordinates after normals
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::cout << " [debug] VBO: triangles=" << polys.size() << ", indices=" << indicies.size() << ", vertices=" << rawVertices.size()/3 << std::endl;
	std::cout << " [debug] Size=" << totalSize << " B" << std::endl;
	std::cout << " [debug]  Vertices=" << sizeOfVertexData << " B" << std::endl;
	std::cout << " [debug]  Normals =" << sizeOfNormalData << " B" << std::endl;
	std::cout << " [debug]  Colors  =" << sizeOfColorData << " B" << std::endl;
	std::cout << " [debug]  Texture =" << sizeOfTextureData << " B" << std::endl;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glNamedBufferData(indexVBO, indicies.size() * sizeof(unsigned short), indicies.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}