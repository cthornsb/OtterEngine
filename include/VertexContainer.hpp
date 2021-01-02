#ifndef VERTEX_CONTAINER_HPP
#define VERTEX_CONTAINER_HPP

#include <vector>

#include "Vertex.hpp"
#include "vector3.hpp"

class object;

class VertexContainer {
public:
	VertexContainer() {}

	Vertex* operator [] (const size_t& index) { return &vertices[index]; }

	void copy(std::vector<float>& out);

	void copy(std::vector<Vertex>& out);

	void reserve(const size_t& nVertices);

	size_t size() const { return vertices.size(); }

	std::vector<Vertex>::iterator begin() { return vertices.begin(); }

	std::vector<Vertex>::iterator end() { return vertices.end(); }

	void append(const std::vector<float>& in, const object* obj);

	void append(const std::vector<Vertex>& in);

	Vertex* back() { return &vertices.back(); }

	void add(const vector3& position, const object* obj);

	void add(const float& posX, const float& posY, const float& posZ, const object* obj);

private:
	std::vector<float> raw;
	std::vector<Vertex> vertices;
};

#endif