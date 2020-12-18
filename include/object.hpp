#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "scene.hpp"
#include "matrix3.hpp"
#include "triangle.hpp"

class object{
public:
	/** Default constructor
	  */
	object();

	/** Object position constructor
	  */	
	object(const vector3& pos_);
	
	/** Get a pointer to the vector of polygons which comprise this 3d object
	  */
	std::vector<triangle>* getPolygons(){ return &polys; }

	/** Get the position offset of the object
	  */
	vector3 getPosition() const { return pos; }

	/** Get the position of the center of the object
	  */
	vector3 getCenter() const { return (center + pos); }

	/** Get the size of the object along the x-axis
	  */
	float getSizeX() const { return (maxSize[0] - minSize[0]); }

	/** Get the size of the object along the y-axis
	  */
	float getSizeY() const { return (maxSize[1] - minSize[1]); }

	/** Get the size of the object along the z-axis
	  */
	float getSizeZ() const { return (maxSize[2] - minSize[2]); }

	/** Get a const pointer to the position vector
	  */
	const vector3* getConstPositionPointer() const { return (const vector3*)&pos; }

	/** Get the number of unique vertices
	  */
	size_t getNumberOfVertices() const { return vertices.size(); }
	
	/** Get the number of unique polygons
	  */
	size_t getNumberOfPolygons() const { return polys.size(); }

	/** Get the number of expected vertices
	  */
	size_t getNumberOfReservedVertices() const { return reservedVertices; }

	/** Get the number of expected polygons
	  */
	size_t getNumberOfReservedPolygons() const { return reservedPolygons; }

	/** Rotate the object by a given amount about the X, Y, and Z, axes (all in radians)
	  * @note This method will rotate vertices from their current position. Use setRotation() to specify the rotation explicitly
	  */
	void rotate(const float& theta, const float& phi, const float& psi);

	/** Move the position of the object
	  * @note This method moves the object relative to its current position. Use setPosition() to specify the position explicitly
	  */
	void move(const vector3 &offset);

	/** Rotate the object to specified angles about the X, Y, and Z, axes (all in radians)
	  */
	void setRotation(const float& theta, const float& phi, const float& psi);

	/** Set the position of the object
	  */
	void setPosition(const vector3 &position);

	/** Reset the coordinates of all vertices to their original values
	  */
	void resetVertices();
	
	/** Reset the offset position of the object to its original location
	  */
	void resetPosition();

	/** Call the child build function for this class
	  */
	void build();

	/** Build this object by adding polygons to the vector of polygons
	  */
	virtual void userBuild() = 0;
	
protected:
	bool built; ///< Flag indicating that the geometry has been constructed

	size_t reservedVertices; ///< The number of expected vertices
	size_t reservedPolygons; ///< The number of expected polygons which will be built

	vector3 pos; ///< The position offset of the object (not necessarily the center)
	vector3 pos0; ///< The original position offset of the object
	
	matrix3 rot; ///< The rotation of the object about the offset position

	vector3 center; ///< Center of the box which bounds the model

	float maxSize[3]; ///< Maximum extent along the x, y, and z-axes
	float minSize[3]; ///< Minimum extent along the x, y, and z-axes

	std::vector<vector3> vertices; ///< Vector of all unique vertices
	std::vector<vector3> vertices0; ///< Vector of all unique vertices with their original coordinates
	
	std::vector<triangle> polys; ///< Vector of all unique polygons which make up this 3d object
	
	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void setSizeX(const float& min_, const float& max_);

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void setSizeY(const float& min_, const float& max_);

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void setSizeZ(const float& min_, const float& max_);

	/** Reserve space in the geometry vectors so that they will not resize when being filled
	  * @param nVert Number of expected vertices
	  * @param nPoly Number of expected polygons. If equal to zero, use the number of vertices
	  */
	void reserve(const size_t& nVert, const size_t& nPoly=0);

	/** Rotate all vertices using the object's internal rotation matrix
	  */
	void transform();
	
	/** Add a unique vertex to the vector of vertices
	  * @return Pointer to the vertex vector
	  */
	vector3* addVertex(const float &x, const float&y, const float&z);

	/** Add a unique vertex to the vector of vertices
	  * @return Pointer to the vertex vector
	  */
	vector3* addVertex(const vector3& vec);

	/** Add a unique polygon to the vector of polygons
	  */
	void addTriangle(const unsigned int &i0, const unsigned int &i1, const unsigned int &i2);

	/** Add two unique polygons (representing a quadrilateral) to the vector of polygons
	  */
	void addQuad(const unsigned int& i0, const unsigned int& i1, const unsigned int& i2, const unsigned int& i3);

	/** Add a unique static triangle to the vector of polygons
	  * @note Static triangles cannot be moved or rotated
	  */
	void addStaticTriangle(const unsigned int& i0, const unsigned int& i1, const unsigned int& i2);

	/** Add two unique static triangles (representing a quadrilateral) to the vector of polygons
	  * @note Static triangles cannot be moved or rotated
	  */
	void addStaticQuad(const unsigned int& i0, const unsigned int& i1, const unsigned int& i2, const unsigned int& i3);
};

#endif
