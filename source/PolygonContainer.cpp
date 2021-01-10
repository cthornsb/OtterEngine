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
	nReservedVertices = N;
	for (size_t i = 0; i < nVertexAttributes; i++) { // Reserve raw data arrays
		rawData[i].reserve(rawNumElements[i] * nReservedVertices);
	}
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
	rawData.clear();
	indicies.clear();
}

void PolygonContainer::addVertex(Vertex* vert) {
	Vector3 pos  = vert->getInitialPosition();
	Vector3 norm = back()->getInitialNormal();
	Vector2 text = vert->getTextureCoordinates();
	ColorRGB color = vert->getColor();

	// Add current vertex index
	//indicies.push_back((unsigned short)(rawData[0].size() / 3));

	// Add vertex position
	rawData[0].push_back(pos[0]);
	rawData[0].push_back(pos[1]);
	rawData[0].push_back(pos[2]);

	rawData[1].push_back(norm[0]);
	rawData[1].push_back(norm[1]);
	rawData[1].push_back(norm[2]);

	rawData[2].push_back(color.r);
	rawData[2].push_back(color.g);
	rawData[2].push_back(color.b);

	rawData[3].push_back(text[0]);
	rawData[3].push_back(text[1]);

	// Increment number of vertices
	nVertices++;
}

bool PolygonContainer::addVertexAttribute(const size_t& nElements) {
	if (nElements == 0 || nElements > 4) // As per OpenGL requirements
		return false;
	rawData.push_back(std::vector<float>());
	rawOffsets.push_back(0);
	rawNumElements.push_back(nElements);
	nVertexAttributes++;
	return true;
}

void PolygonContainer::setupVBOs() {
	// Generate vertex VBO
	glCreateBuffers(1, &vertexVBO);
	glCreateBuffers(1, &indexVBO);

	std::cout << " [debug] vertexVBO=" << vertexVBO << ", indexVBO=" << indexVBO << std::endl;
	
	// Compute the total
	nTotalNumberOfBytes = 0;
	std::vector<size_t> rawLengths(nVertexAttributes, 0);
	for (size_t i = 0; i < nVertexAttributes; i++) {
		rawLengths[i] = rawData[i].size() * sizeof(float);
		rawOffsets[i] = nTotalNumberOfBytes;
		nTotalNumberOfBytes += rawLengths[i];
	}
	
	// Bind buffer and reserve vertex buffer memory
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glNamedBufferData(vertexVBO, nTotalNumberOfBytes, 0x0, GL_STATIC_DRAW);

	// Copy data to GPU
	for (size_t i = 0; i < nVertexAttributes; i++) {
		glNamedBufferSubData(vertexVBO, (GLintptr)rawOffsets[i], (GLsizeiptr)rawLengths[i], (const void*)rawData[i].data());
		std::cout << " [debug]  (location = " << i <<") elements=" << rawNumElements[i] << ", size=" << rawLengths[i] << " B" << std::endl;
	}

	// Unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Bind indices buffer
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glNamedBufferData(indexVBO, indicies.size() * sizeof(unsigned short), indicies.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

	std::cout << " [debug] VBO: triangles=" << polys.size() << ", vertices=" << nVertices << std::endl;
}