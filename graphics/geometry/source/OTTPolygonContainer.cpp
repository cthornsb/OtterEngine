#include "OTTPolygonContainer.hpp"
#include "OTTVertex.hpp"
#include "OTTVertexContainer.hpp"

#include <graphics/core/OTTWindow.hpp>

#include <iostream>
#include <GL/glew.h>

ott::PolygonContainer::~PolygonContainer() {
	if (vertexVBO) {
		glDeleteBuffers(1, &vertexVBO);
	}
}

void ott::PolygonContainer::Reserve(const size_t& N) {
	nReservedVertices = N;
	for (size_t i = 0; i < nVertexAttributes; i++) { // Reserve raw data arrays
		rawData[i].reserve(rawNumElements[i] * nReservedVertices);
	}
	polys.reserve(N);
}

void ott::PolygonContainer::Add(const Polygon& tri) {
	polys.push_back(tri);
	this->AddVertex(tri.p2);
	this->AddVertex(tri.p1);
	this->AddVertex(tri.p0);
}

void ott::PolygonContainer::Add(const Vertex* v0, const Vertex* v1, const Vertex* v2, const Object* obj) {
	polys.push_back(Polygon(v0, v1, v2, obj));
	this->AddVertex(v2);
	this->AddVertex(v1);
	this->AddVertex(v0);
}

void ott::PolygonContainer::Add(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, VertexContainer* vertices, const Object* obj) {
	polys.push_back(Polygon((*vertices)[i0], (*vertices)[i1], (*vertices)[i2], obj));
	this->AddVertex((*vertices)[i2]);
	this->AddVertex((*vertices)[i1]);
	this->AddVertex((*vertices)[i0]);
}

void ott::PolygonContainer::ModifyTextureMap(const Vector2& uv0, const Vector2& uv1, const Vector2& uv2) {
	auto iter = rawData[2].end() - 6;
	*iter = uv0[0]; iter++;
	*iter = uv0[1]; iter++;
	*iter = uv1[0]; iter++;
	*iter = uv1[1]; iter++;
	*iter = uv2[0]; iter++;
	*iter = uv2[1]; iter++;
}

void ott::PolygonContainer::ModifyNormalVector(const Vector3& n0, const Vector3& n1, const Vector3& n2) {
	auto iter = rawData[1].end() - 9;
	*iter = n0[0]; iter++;
	*iter = n0[1]; iter++;
	*iter = n0[2]; iter++;
	*iter = n1[0]; iter++;
	*iter = n1[1]; iter++;
	*iter = n1[2]; iter++;
	*iter = n2[0]; iter++;
	*iter = n2[1]; iter++;
	*iter = n2[2]; iter++;
}

void ott::PolygonContainer::Finalize() {
	if (polys.empty()) // No geometry
		return;
	// Setup buffer objects
	this->Setup();	
	rawData.clear();
}

void ott::PolygonContainer::AddVertex(const Vertex* vert) {
	Vector3 pos  = vert->pos0;
	Vector3 norm = this->Back()->InitialNormal();
	Vector2 uv = vert->texCoords;

	// Add Vertex position
	rawData[0].push_back(pos[0]);
	rawData[0].push_back(pos[1]);
	rawData[0].push_back(pos[2]);

	rawData[1].push_back(norm[0]);
	rawData[1].push_back(norm[1]);
	rawData[1].push_back(norm[2]);

	rawData[2].push_back(uv[0]);
	rawData[2].push_back(uv[1]);

	// Increment number of vertices
	nVertices++;
}

bool ott::PolygonContainer::AddVertexAttribute(const size_t& nElements) {
	if (nElements == 0 || nElements > 4) { // As per OpenGL requirements
		return false;
	}
	rawData.push_back(std::vector<float>());
	rawNumElements.push_back(nElements);
	nVertexAttributes++;
	return true;
}

void ott::PolygonContainer::Setup() {
	/*
	 * Generate vertex buffer object (VBO)
	 */
	vertexVBO = ott::GenerateVertexBufferObject(rawData, rawOffsets);

	if (bDebugMode) {
		std::cout << " [debug] vertexVBO=" << vertexVBO << std::endl;
		std::cout << " [debug] VBO: triangles=" << polys.size() << ", vertices=" << nVertices << std::endl;
	}
}

void ott::PolygonContainer::Free() {
	polys.clear();
	polys.shrink_to_fit();
	nReservedVertices = 0;
}
