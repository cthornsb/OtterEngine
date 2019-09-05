#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "matrix3.hpp"
#include "triangle.hpp"

class object{
public:
	/** Default constructor
	  */
	object() { }

	/** Object position constructor
	  */	
	object(const vector3 &pos_) : pos(pos_), pos0(pos_) { }
	
	/** Get a pointer to the vector of polygons which comprise this 3d object
	  */
	std::vector<triangle>* getPolygons(){ return &polys; }

	/** Get the position offset of the object
	  */
	vector3 getPosition() const { return pos; }

	/** Get the number of unique vertices
	  */
	size_t getNumberOfVertices() const { return vertices.size(); }
	
	/** Get the number of unique polygons
	  */
	size_t getNumberOfPolygons() const { return polys.size(); }

	/** Rotate the object by a given amount about the X, Y, and Z, axes (all in radians)
	  * @note This method will rotate vertices from their current position. Use setRotation() to specify the rotation explicitly
	  */
	void rotate(const double &theta, const double &phi, const double &psi);

	/** Move the position of the object
	  * @note This method moves the object relative to its current position. Use setPosition() to specify the position explicitly
	  */
	void move(const vector3 &offset);

	/** Rotate the object to specified angles about the X, Y, and Z, axes (all in radians)
	  */
	void setRotation(const double &theta, const double &phi, const double &psi);

	/** Set the position of the object
	  */
	void setPosition(const vector3 &position);

	/** Reset the coordinates of all vertices to their original values
	  */
	void resetVertices();
	
	/** Reset the offset position of the object to its original location
	  */
	void resetPosition();

	/** Build this object by adding polygons to the vector of polygons
	  */
	virtual void build() = 0;
	
protected:
	vector3 pos; ///< The position offset of the object (not necessarily the center)
	vector3 pos0; ///< The original position offset of the object
	
	matrix3 rot; ///< The rotation of the object about the offset position
	
	std::vector<vector3> vertices; ///< Vector of all unique vertices
	std::vector<vector3> vertices0; ///< Vector of all unique vertices with their original coordinates
	
	std::vector<triangle> polys; ///< Vector of all unique polygons which make up this 3d object
	
	/** Rotate all vertices using the object's internal rotation matrix
	  */
	void transform();
	
	/** Add a unique vertex to the vector of vertices
	  */
	void addVertex(const double &x, const double &y, const double &z);
	
	/** Add a unique polygon to the vector of polygons
	  */
	void addPolygon(const size_t &i0, const size_t &i1, const size_t &i2);
};

#endif
