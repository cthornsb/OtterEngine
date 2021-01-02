#include "PolygonContainer.hpp"
#include "triangle.hpp"
#include "object.hpp"

/*void PolygonContainer::copy(std::vector<unsigned short>& out) {

}

void PolygonContainer::copy(std::vector<Vertex>& out) {

}*/

void PolygonContainer::reserve(const size_t& N) {
	raw.reserve(N * 3);
	polys.reserve(N);
}

/*void PolygonContainer::append(const std::vector<unsigned short>& in, const object* obj) {

}

void PolygonContainer::append(const std::vector<Vertex>& in) {

}*/

void PolygonContainer::add(const triangle& tri, VertexContainer* vertices) {
	size_t index;
	vertices->findVertex(tri.p0, index);
	raw.push_back((unsigned short)index);
	vertices->findVertex(tri.p1, index);
	raw.push_back((unsigned short)index);
	vertices->findVertex(tri.p2, index);
	raw.push_back((unsigned short)index);
	polys.push_back(tri);
}

void PolygonContainer::add(Vertex* v0, Vertex* v1, Vertex* v2, VertexContainer* vertices, const object* obj) {
	size_t index;
	vertices->findVertex(v0, index);
	raw.push_back((unsigned short)index);
	vertices->findVertex(v1, index);
	raw.push_back((unsigned short)index);
	vertices->findVertex(v2, index);
	raw.push_back((unsigned short)index);
	polys.push_back(triangle(v0, v1, v2, obj));
}

void PolygonContainer::add(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, VertexContainer* vertices, const object* obj) {
	raw.push_back(i0);
	raw.push_back(i1);
	raw.push_back(i2);
	polys.push_back(triangle((*vertices)[i0], (*vertices)[i1], (*vertices)[i2], obj));
}