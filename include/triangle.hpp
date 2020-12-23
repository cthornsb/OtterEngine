#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "Vertex.hpp"
#include "plane.hpp"

class triangle : public plane {
public:
	Vertex* p0; ///< Pointer to one of the vertices
	Vertex* p1; ///< Pointer to one of the vertices
	Vertex* p2; ///< Pointer to one of the vertices

	const vector3* offset; ///< Object position offset 

	/** Default constructor
	  */
	triangle() : 
		plane(), 
		p0(0x0), 
		p1(0x0),
		p2(0x0),
		offset(0x0)
	{ 
	}
	
	/** Pointer constructor
	  */
	triangle(Vertex* p0_, Vertex* p1_, Vertex* p2_) :
		plane(p0_->pos, p1_->pos, p2_->pos),
		p0(p0_),
		p1(p1_),
		p2(p2_),
		offset(&zeroVector)
	{
	}

	/** Position / normal pointer constructor
	  */
	triangle(Vertex* p0_, Vertex* p1_, Vertex* p2_, const vector3* offset_) :
		plane(p0_->pos, p1_->pos, p2_->pos),
		p0(p0_),
		p1(p1_),
		p2(p2_),
		offset(offset_)
	{
	}

	vector3 getCenterPoint() const;

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
