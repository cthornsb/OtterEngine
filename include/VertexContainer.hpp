#ifndef VERTEX_CONTAINER_HPP
#define VERTEX_CONTAINER_HPP

#include <vector>

#include "Vertex.hpp"
#include "vector3.hpp"

class object;

class VertexContainer {
public:
	VertexContainer() :
		nVertices(0),
		raw(),
		vertices()	
	{
	}

	Vertex* operator [] (const size_t& index) { return &vertices[index]; }

	void copy(std::vector<float>& out);

	void copy(std::vector<Vertex>& out);

	void reserve(const size_t& N);

	bool findVertex(Vertex* ptr, size_t& index) const;
	
	size_t size() const { return nVertices; }

	std::vector<Vertex>::iterator begin() { return vertices.begin(); }

	std::vector<Vertex>::iterator end() { return vertices.end(); }

	void append(const std::vector<float>& in, const object* obj);

	void append(const std::vector<Vertex>& in);

	Vertex* back() { return &vertices.back(); }

	float* getRawData() { return raw.data(); }

	const float* getConstRawData() const { return (const float*)raw.data(); }

	void add(const vector3& position, const object* obj);

	void add(const float& posX, const float& posY, const float& posZ, const object* obj);

private:
	size_t nVertices; ///< Number of vertices

	std::vector<float> raw; ///< Raw vertex positions

	std::vector<Vertex> vertices; ///< Vertex information
};

#endif