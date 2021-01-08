#ifndef VERTEX_CONTAINER_HPP
#define VERTEX_CONTAINER_HPP

#include <vector>

#include "Vertex.hpp"
#include "Vector.hpp"

class object;

class VertexContainer {
public:
	VertexContainer() :
		nVertices(0),
		vertices()	
	{
	}

	Vertex* operator [] (const size_t& index) { return &vertices[index]; }

	void copy(std::vector<Vertex>& out);

	void reserve(const size_t& N);

	bool findVertex(Vertex* ptr, size_t& index) const;
	
	size_t size() const { return nVertices; }

	std::vector<Vertex>::iterator begin() { return vertices.begin(); }

	std::vector<Vertex>::iterator end() { return vertices.end(); }

	void append(const std::vector<float>& in, const object* obj);

	void append(const std::vector<Vertex>& in);

	Vertex* back() { return &vertices.back(); }

	void add(const Vector3& position, const object* obj);

	void add(const float& posX, const float& posY, const float& posZ, const object* obj);

private:
	size_t nVertices; ///< Number of vertices

	std::vector<Vertex> vertices; ///< Vertex information
};

#endif