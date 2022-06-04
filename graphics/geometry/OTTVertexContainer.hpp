#ifndef GRAPHICS_GEOMETRY_OTT_VERTEX_CONTAINER_HPP
#define GRAPHICS_GEOMETRY_OTT_VERTEX_CONTAINER_HPP

#include "OTTVertex.hpp"

#include <math/OTTVector.hpp>

#include <vector>

namespace ott {

class Object;

class VertexContainer {
public:

	VertexContainer() :
		nVertices(0),
		vertices()	
	{
	}

	const Vertex* operator [] (const size_t& index) {

		return &vertices[index]; 
	}

	void Copy(std::vector<Vertex>& out);

	void Reserve(const size_t& N);

	bool FindVertex(Vertex* ptr, size_t& index) const;
	
	size_t size() const { 
		
		return nVertices; 
	}

	size_t capacity() const { 
		
		return vertices.capacity(); 
	}

	std::vector<Vertex>::iterator Begin() {

		return vertices.begin(); 
	}

	std::vector<Vertex>::iterator End() {

		return vertices.end(); 
	}

	void Append(const std::vector<float>& in, const Object* obj);

	void Append(const std::vector<Vertex>& in);

	Vertex* Back() { 
		
		return &vertices.back(); 
	}

	void Add(const Vector3& position, const Object* obj);

	void Add(const float& posX, const float& posY, const float& posZ, const Object* obj);

	void Free();

private:

	size_t nVertices; ///< Number of vertices

	std::vector<Vertex> vertices; ///< Vertex information
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_VERTEX_CONTAINER_HPP