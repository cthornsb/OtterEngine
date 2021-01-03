#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "Vertex.hpp"
#include "plane.hpp"

class triangle : public plane {
public:
	Vertex* p0; ///< Pointer to one of the vertices
	Vertex* p1; ///< Pointer to one of the vertices
	Vertex* p2; ///< Pointer to one of the vertices

	/** Default constructor
	  */
	triangle() : 
		plane(), 
		p0(0x0), 
		p1(0x0),
		p2(0x0),
		offset(0x0),
		rotation(0x0),
		parent(0x0)
	{ 
	}
	
	/** Pointer constructor
	  */
	triangle(Vertex* p0_, Vertex* p1_, Vertex* p2_) :
		plane(p0_->pos0, p1_->pos0, p2_->pos0),
		p0(p0_),
		p1(p1_),
		p2(p2_),
		offset(&zeroVector),
		rotation(0x0),
		parent(0x0)
	{
	}

	/** Position / normal pointer constructor
	  */
	triangle(Vertex* p0_, Vertex* p1_, Vertex* p2_, const object* obj);

	/** Check if this triangle contains a specified vertex
	  */
	bool hasVertex(Vertex* ptr) { return (ptr == p0 || ptr == p1 || ptr == p2); }

	/** Get the center point of the triangle
	  */
	vector3 getCenterPoint() const;

	vector3 getInitialCenterPoint() const { return p; }

	/** Get the normal vector at the center point of the triangle
	  */
	vector3 getNormal() const;

	vector3 getInitialNormal() const { return norm; }

	/** Update the infinite plane which bounds this triangle. This method sets the "position" of the plane to the
	  * center-of-mass of the three triangle vertices and computes the normal to the surface of the triangle
	  * assuming clockwise orientation of @a p0, @a p1, and @a p2
	  */
	void update();

	/** Dump the vertex coordinates to stdout
	  */
	void dump() const ;

private:
	const vector3* offset; ///< Vertex position offset

	const matrix3* rotation; ///< Rotation matrix of object

	const object* parent; ///< Pointer to the parent object
};

#endif
