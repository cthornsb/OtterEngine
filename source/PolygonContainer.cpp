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
	indicies.clear();
}

void PolygonContainer::addVertex(Vertex* vert) {
	Vector3 pos  = vert->getInitialPosition();
	Vector3 norm = back()->getInitialNormal();
	indicies.push_back((unsigned short)(rawVertices.size() / 3));
	rawVertices.push_back(pos[0]);
	rawVertices.push_back(pos[1]);
	rawVertices.push_back(pos[2]);
	rawNormals.push_back(norm[0]);
	rawNormals.push_back(norm[1]);
	rawNormals.push_back(norm[2]);
}

bool PolygonContainer::addIndex(Vertex* vert, VertexContainer* vertices) {
	size_t index;
	if (vertices->findVertex(vert, index)) {
		indicies.push_back((unsigned short)index);
		//addVertex(vert);
		return true;
	}
	else
		std::cout << " [error] failed to find vertex pointer in vertex array!" << std::endl;
	return false;
}

void PolygonContainer::setupVBOs() {
	// Generate vertex VBO
	glCreateBuffers(1, &vertexVBO);
	glCreateBuffers(1, &indexVBO);

	std::cout << " [debug] vertexVBO=" << vertexVBO << ", indexVBO=" << indexVBO << std::endl;

	sizeOfVertexData = rawVertices.size() * sizeof(float);
	sizeOfNormalData = rawNormals.size() * sizeof(float);
	//sizeOfColorData = 
	//sizeOfTextureData =

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glNamedBufferData(vertexVBO, sizeOfVertexData + sizeOfNormalData, 0x0, GL_STATIC_DRAW); // Reserve buffer memory
	glNamedBufferSubData(vertexVBO, 0, sizeOfVertexData, rawVertices.data()); // Copy vertices
	glNamedBufferSubData(vertexVBO, sizeOfVertexData, sizeOfNormalData, rawNormals.data()); // Copy normals after vertices
	//glNamedBufferSubData(vertexVBO, vSize + nSize, cSize, colors); // Copy colors after norms
	//glNamedBufferSubData(vertexVBO, vSize + nSize + cSize, tSize, texCoords); // Copy texture coordinates after colors
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::cout << " [debug] VBO: triangles=" << polys.size() << ", indices=" << indicies.size() << ", vertices=" << rawVertices.size() << std::endl;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glNamedBufferData(indexVBO, indicies.size() * sizeof(unsigned short), indicies.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}