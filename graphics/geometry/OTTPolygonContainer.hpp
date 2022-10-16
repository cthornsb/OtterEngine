#ifndef GRAPHICS_GEOMETRY_OTT_POLYGON_CONTAINER_HPP
#define GRAPHICS_GEOMETRY_OTT_POLYGON_CONTAINER_HPP

#include "OTTPolygon.hpp"

#include <cstdint>
#include <vector>

namespace ott {

class Object;
class VertexContainer;

class PolygonContainer {
public:
	PolygonContainer() :
		bDebugMode(false),
		vertexVBO(0),
		nReservedVertices(0),
		nTotalNumberOfBytes(0),
		nVertexAttributes(0),
		nVertices(0),
		rawData(),
		rawOffsets(),
		rawNumElements(),
		polys()
	{
		// Add vertex position to VBO
		this->AddVertexAttribute(3); //position
		this->AddVertexAttribute(3); //normal
		this->AddVertexAttribute(2); //texture
	}

	~PolygonContainer();

	Polygon* operator [] (const size_t& index) {
		return &polys[index];
	}

	void Copy(std::vector<uint16_t>& out);

	void Copy(std::vector<Vertex>& out);

	void Reserve(const size_t& N);

	uint32_t VertexVBO() const {
		return vertexVBO;
	}

	bool Empty() const {
		return polys.empty();
	}

	size_t Size() const {
		return polys.size();
	}

	size_t Capacity() const {
		return polys.capacity();
	}

	std::vector<Polygon>::iterator Begin() {
		return polys.begin();
	}

	std::vector<Polygon>::iterator End() {
		return polys.end();
	}

	void Append(const std::vector<uint16_t>& in, const Object* obj);

	void Append(const std::vector<Vertex>& in);

	Polygon* Back() {
		return &polys.back();
	}

	std::vector<Polygon>* Polygons() {
		return &polys;
	}

	const std::vector<Polygon>* ConstPolygons() const {
		return &polys;
	}

	size_t NumberOfVertexAttributes() const {
		return nVertexAttributes;
	}

	size_t RawDataLength(const size_t& index) const {
		return rawData.at(index).size();
	}

	size_t RawDataOffset(const size_t& index) const {
		return rawOffsets.at(index);
	}

	size_t NumberOfAttributeElements(const size_t& index) const {
		return rawNumElements.at(index);
	}

	size_t NumberOfVertices() const {
		return nVertices;
	}

	void Add(const Polygon& tri);

	void Add(const Vertex* v0, const Vertex* v1, const Vertex* v2, const Object* obj);

	void Add(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, VertexContainer* vertices, const Object* obj);

	/** Set UV texture mapping coordinates for the most recently added triangle
	  */
	void ModifyTextureMap(const Vector2& uv0, const Vector2& uv1, const Vector2& uv2);

	/** Set vertex normals for the most recently added triangle
	  */
	void ModifyNormalVector(const Vector3& n0, const Vector3& n1, const Vector3& n2);

	void Finalize();

	void Free();

private:
	bool bDebugMode;

	uint32_t vertexVBO;

	size_t nReservedVertices; ///< Number of vertices reserved in memory

	size_t nTotalNumberOfBytes; ///< Total number of bytes in object VBO

	size_t nVertexAttributes; ///< Number of vertex attributes defined

	size_t nVertices; ///< Total number of vertices

	std::vector<std::vector<float> > rawData; ///< Raw vertex attribute information

	std::vector<size_t> rawOffsets; ///< Raw vertex attribute array offsets (in Bytes)

	std::vector<size_t> rawNumElements; ///< Number of elements for each vertex attribute

	std::vector<Polygon> polys; ///< Polygon information

	void AddVertex(const Vertex* vert);
	
	/** Add a vertex attribute
	  * @param nElements Number of elements per vertex for this attribute (as per OpenGL requirements, must be 1, 2, 3, or 4)
	  */
	bool AddVertexAttribute(const size_t& nElements);

	void Setup();
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_POLYGON_CONTAINER_HPP
