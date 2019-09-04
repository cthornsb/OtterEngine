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
	object(const vector3 &pos_) : pos(pos_) { }
	
	/** Build this object by adding polygons to the vector of polygons
	  */
	virtual void build() = 0;

	/** Get a pointer to the vector of polygons which comprise this 3d object
	  */
	std::vector<triangle>* getPolygons(){ return &polys; }

	/** Rotate the object by a given amount about the X, Y, and Z, axes (all in radians)
	  */
	void rotate(const double &theta, const double &phi, const double &psi);
	
protected:
	vector3 pos; ///< The position offset of the object (not necessarily the center)
	
	matrix3 rot; ///< The rotation of the object about the offset position
	
	std::vector<triangle> polys; ///< Vector of polygons which make up this 3d object
};

#endif
