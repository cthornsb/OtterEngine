#ifndef POLYGON_CONTAINER_HPP
#define POLYGON_CONTAINER_HPP

#include <vector>

#include "triangle.hpp"
//include "vector3.hpp"

class object;
class VertexContainer;

class PolygonContainer {
public:
	PolygonContainer() :
		raw(),
		polys()
	{
	}

	triangle* operator [] (const size_t& index) { return &polys[index]; }

	void copy(std::vector<unsigned short>& out);

	void copy(std::vector<Vertex>& out);

	void reserve(const size_t& N);

	size_t size() const { return polys.size(); }

	std::vector<triangle>::iterator begin() { return polys.begin(); }

	std::vector<triangle>::iterator end() { return polys.end(); }

	void append(const std::vector<unsigned short>& in, const object* obj);

	void append(const std::vector<Vertex>& in);

	triangle* back() { return &polys.back(); }

	unsigned short* getIndicies() { return raw.data(); }

	const unsigned short* getConstIndicies() const { return (const unsigned short*)raw.data(); }

	std::vector<triangle>* getPolygons() { return &polys; }

	const std::vector<triangle>* getConstPolygons() const { return &polys; }

	void add(const triangle& tri, VertexContainer* vertices);

	void add(Vertex* v0, Vertex* v1, Vertex* v2, VertexContainer* vertices, const object* obj);

	void add(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, VertexContainer* vertices, const object* obj);

private:
	std::vector<unsigned short> raw; ///< Raw vertex indices

	std::vector<triangle> polys; ///< Polygon information
};

#endif

