#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "matrix3.hpp"
#include "plane.hpp"

class triangle : public plane {
public:
	const vector3* p0; ///< Pointer to one of the vertices
	const vector3* p1; ///< Pointer to one of the vertices
	const vector3* p2; ///< Pointer to one of the vertices

	/** Default constructor
	  */
	triangle() : plane(), p0(), p1(), p2() { }
	
	/** Vertex constructor
	  */
	triangle(const vector3 &p0_, const vector3 &p1_, const vector3 &p2_) : plane(p0_, p1_, p2_), p0(&p0_), p1(&p1_), p2(&p2_) { }

	/** Update the infinite plane which bounds this triangle. This method sets the "position" of the plane to the
	  * center-of-mass of the three triangle vertices and computes the normal to the surface of the triangle
	  * assuming clockwise orientation of @a p0, @a p1, and @a p2
	  */
	void update();

	/** Dump the vertex coordinates to stdout
	  */
	void dump() const ;
};

#endif
