#include "VertexContainer.hpp"

void VertexContainer::copy(std::vector<float>& out) {
	for (auto vert = raw.cbegin(); vert != raw.cend(); vert++)
		out.push_back(*vert);
}

void VertexContainer::copy(std::vector<Vertex>& out) {
	for (auto vert = vertices.cbegin(); vert != vertices.cend(); vert++)
		out.push_back(*vert);
}

void VertexContainer::reserve(const size_t& nVertices) {
	raw.reserve(nVertices * 3);
	vertices.reserve(nVertices);
}

void VertexContainer::append(const std::vector<float>& in, const object* obj) {
	int index = 0;
	vector3 vec;
	for (auto vert = in.cbegin(); vert != in.cend(); vert++) {
		raw.push_back(*vert);
		switch (++index % 3) {
		case 0: // z
			vec.z = *vert;
			break;
		case 1: // y
			vec.y = *vert;
			break;
		case 2: // x
			vec.x = *vert;
			vertices.push_back(Vertex(vec, obj));
			break;
		default:
			break;
		}
	}
}

void VertexContainer::append(const std::vector<Vertex>& in) {
	for (auto vert = in.cbegin(); vert != in.cend(); vert++) {
		raw.push_back(vert->getPosition().x);
		raw.push_back(vert->getPosition().y);
		raw.push_back(vert->getPosition().z);
		vertices.push_back(*vert);
	}
}

void VertexContainer::add(const vector3& position, const object* obj) {
	raw.push_back(position.x);
	raw.push_back(position.y);
	raw.push_back(position.z);
	vertices.push_back(Vertex(position, obj));
}

void VertexContainer::add(const float& posX, const float& posY, const float& posZ, const object* obj) {
	raw.push_back(posX);
	raw.push_back(posY);
	raw.push_back(posZ);
	vertices.push_back(Vertex(vector3(posX, posY, posZ), obj));
}