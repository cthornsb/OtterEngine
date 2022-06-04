#include "OTTVertexContainer.hpp"

void ott::VertexContainer::Copy(std::vector<Vertex>& out) {
	for (auto vert = vertices.cbegin(); vert != vertices.cend(); vert++)
		out.push_back(*vert);
}

void ott::VertexContainer::Reserve(const size_t& N) {
	vertices.reserve(N);
}

bool ott::VertexContainer::FindVertex(Vertex* ptr, size_t& index) const {
	index = 0;
	for (auto vert = vertices.cbegin(); vert != vertices.cend(); vert++) {
		if (&(*vert) == ptr)
			return true;
		index++;
	}
	return false;
}

void ott::VertexContainer::Append(const std::vector<float>& in, const Object* obj) {
	int32_t index = 0;
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

void ott::VertexContainer::Append(const std::vector<Vertex>& in) {
	for (auto vert = in.cbegin(); vert != in.cend(); vert++) {
		vertices.push_back(*vert);
		nVertices++;
	}
}

void ott::VertexContainer::Add(const Vector3& position, const Object* obj) {
	vertices.push_back(Vertex(position, obj));
	nVertices++;
}

void ott::VertexContainer::Add(const float& posX, const float& posY, const float& posZ, const Object* obj) {
	vertices.push_back(Vertex(Vector3(posX, posY, posZ), obj));
	nVertices++;
}

void ott::VertexContainer::Free() {
	vertices.clear(); 
	vertices.shrink_to_fit();
	nVertices = 0;
}
