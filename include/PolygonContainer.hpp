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
		vertexVBO(0),
		indexVBO(0),
		sizeOfVertexData(0),
		sizeOfNormalData(0),
		rawVertices(),
		indicies(),
		polys()
	{
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

	const float* getConstVertexData() const { return (const float*)rawVertices.data(); }

	const unsigned short* getConstIndicies() const { return (const unsigned short*)indicies.data(); }

	std::vector<triangle>* getPolygons() { return &polys; }

	const std::vector<triangle>* getConstPolygons() const { return &polys; }

	size_t getSizeOfVertexData() const { return sizeOfVertexData; }

	size_t getSizeOfNormalData() const { return sizeOfNormalData; }

	//size_t getSizeOfColorData() const { return sizeOfColorData; }

	//size_t getSizeOfTextureData() const { return sizeOfTextureData; }

	void add(const triangle& tri, VertexContainer* vertices);

	void add(Vertex* v0, Vertex* v1, Vertex* v2, VertexContainer* vertices, const object* obj);

	void add(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, VertexContainer* vertices, const object* obj);

	void finalize();

private:
	unsigned int vertexVBO;
	unsigned int indexVBO;

	size_t sizeOfVertexData;
	size_t sizeOfNormalData;
	//size_t sizeOfColorData;
	//size_t sizeOfTextureData;

	std::vector<float> rawVertices; ///< Raw vertex information

	std::vector<float> rawNormals; ///< Raw vertex normal information

	std::vector<unsigned short> indicies; ///< Polygon vertex indicies

	std::vector<triangle> polys; ///< Polygon information

	void addVertex(Vertex* vert);

	bool addIndex(Vertex* vert, VertexContainer* vertices);

	void setupVBOs();
};

#endif

