#include "VertexContainer.hpp"

void VertexContainer::copy(std::vector<Vertex>& out) {
	for (auto vert = vertices.cbegin(); vert != vertices.cend(); vert++)
		out.push_back(*vert);
}

void VertexContainer::reserve(const size_t& N) {
	vertices.reserve(N);
}

bool VertexContainer::findVertex(Vertex* ptr, size_t& index) const {
	index = 0;
	for (auto vert = vertices.cbegin(); vert != vertices.cend(); vert++) {
		if (&(*vert) == ptr)
			return true;
		index++;
	}
	return false;
}

void VertexContainer::append(const std::vector<float>& in, const object* obj) {
	int index = 0;
	Vector3 vec;
	for (auto vert = in.cbegin(); vert != in.cend(); vert++) {
		switch (++index % 3) {
		case 0: // z
			vec[2] = *vert;
			break;
		case 1: // y
			vec[1] = *vert;
			break;
		case 2: // x
			vec[0] = *vert;
			vertices.push_back(Vertex(vec, obj));
			nVertices++;
			break;
		default:
			break;
		}
	}
}

void VertexContainer::append(const std::vector<Vertex>& in) {
	for (auto vert = in.cbegin(); vert != in.cend(); vert++) {
		vertices.push_back(*vert);
		nVertices++;
	}
}

void VertexContainer::add(const Vector3& position, const object* obj) {
	vertices.push_back(Vertex(position, obj));
	nVertices++;
}

void VertexContainer::add(const float& posX, const float& posY, const float& posZ, const object* obj) {
	vertices.push_back(Vertex(Vector3(posX, posY, posZ), obj));
	nVertices++;
}

void VertexContainer::free() {
	vertices.clear(); 
	vertices.shrink_to_fit();
	nVertices = 0;
}