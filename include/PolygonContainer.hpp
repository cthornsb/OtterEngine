#ifndef POLYGON_CONTAINER_HPP
#define POLYGON_CONTAINER_HPP

#include <vector>
#include <memory>

#include "triangle.hpp"

class object;
class VertexContainer;

class PolygonContainer {
public:
	PolygonContainer() :
		vertexVBO(0),
		indexVBO(0),
		nReservedVertices(0),
		nTotalNumberOfBytes(0),
		nVertexAttributes(0),
		nVertices(0),
		rawData(),
		rawOffsets(),
		rawNumElements(),
		indicies(),
		polys()
	{
		// Add vertex position to VBO
		addVertexAttribute(3); //position
		addVertexAttribute(3); //normal
		addVertexAttribute(3); //color
		addVertexAttribute(2); //texture
	}

	~PolygonContainer();

	triangle* operator [] (const size_t& index) { return &polys[index]; }

	void copy(std::vector<unsigned short>& out);

	void copy(std::vector<Vertex>& out);

	void reserve(const size_t& N);

	unsigned int getVertexVBO() const { return vertexVBO; }

	unsigned int getIndexVBO() const { return indexVBO; }

	size_t size() const { return polys.size(); }

	std::vector<triangle>::iterator begin() { return polys.begin(); }

	std::vector<triangle>::iterator end() { return polys.end(); }

	void append(const std::vector<unsigned short>& in, const object* obj);

	void append(const std::vector<Vertex>& in);

	triangle* back() { return &polys.back(); }

	std::vector<triangle>* getPolygons() { return &polys; }

	const std::vector<triangle>* getConstPolygons() const { return &polys; }

	size_t getNumberOfVertexAttributes() const { return nVertexAttributes; }

	size_t getRawDataLength(const size_t& index) const { return rawData.at(index).size(); }

	size_t getRawDataOffset(const size_t& index) const { return rawOffsets.at(index); }

	size_t getNumberOfAttributeElements(const size_t& index) const { return rawNumElements.at(index); }

	size_t getNumberOfVertices() const { return nVertices; }

	void add(const triangle& tri, VertexContainer* vertices);

	void add(Vertex* v0, Vertex* v1, Vertex* v2, VertexContainer* vertices, const object* obj);

	void add(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, VertexContainer* vertices, const object* obj);

	void finalize();

private:
	unsigned int vertexVBO;
	unsigned int indexVBO;

	size_t nReservedVertices; ///< Number of vertices reserved in memory

	size_t nTotalNumberOfBytes; ///< Total number of bytes in object VBO

	size_t nVertexAttributes; ///< Number of vertex attributes defined

	size_t nVertices; ///< Total number of vertices

	std::vector<std::vector<float> > rawData; ///< Raw vertex attribute information

	std::vector<size_t> rawOffsets; ///< Raw vertex attribute array offsets (in Bytes)

	std::vector<size_t> rawNumElements; ///< Number of elements for each vertex attribute

	std::vector<unsigned short> indicies; ///< Polygon vertex indicies

	std::vector<triangle> polys; ///< Polygon information

	void addVertex(Vertex* vert);
	
	/** Add a vertex attribute
	  * @param nElements Number of elements per vertex for this attribute (as per OpenGL requirements, must be 1, 2, 3, or 4)
	  */
	bool addVertexAttribute(const size_t& nElements);

	void setupVBOs();
};

#endif

