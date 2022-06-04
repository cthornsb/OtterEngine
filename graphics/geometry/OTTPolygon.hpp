#ifndef GRAPHICS_GEOMETRY_OTT_POLYGON_HPP
#define GRAPHICS_GEOMETRY_OTT_POLYGON_HPP

#include "OTTPlane.hpp"

#include <math/OTTVector.hpp>

namespace ott {

class Vertex;

class Object;
class Matrix3;

class Polygon : public Plane {
public:

	const Vertex* const p0;
	const Vertex* const p1;
	const Vertex* const p2;

	/** Default constructor
	  */
	Polygon() :
		Plane(),
		p0(nullptr),
		p1(nullptr),
		p2(nullptr),
		offset(nullptr),
		rotation(nullptr),
		parent(nullptr)
	{ 
	}

	/** Pointer constructor
	  */
	Polygon(const Vertex* p0_, const Vertex* p1_, const Vertex* p2_);

	/** Position / normal pointer constructor
	  */
	Polygon(const Vertex* p0_, const Vertex* p1_, const Vertex* p2_, const Object* obj);

	/** Get the center point of the triangle
	  */
	Vector3 CenterPoint() const ;

	Vector3 InitialCenterPoint() const { 

		return this->pos; 
	}

	/** Get the normal vector at the center point of the triangle
	  */
	Vector3 Normal() const ;

	Vector3 InitialNormal() const { 

		return this->norm; 
	}

	/** Check if this triangle contains a specified vertex
	  */
	bool ContainsVertex(Vertex* ptr) const ;

	/**
	  */
	void OffsetCenter(Vector3 displacement);

	/** Update the infinite plane which bounds this triangle. This method sets the "position" of the plane to the
	  * center-of-mass of the three triangle vertices and computes the normal to the surface of the triangle
	  * assuming clockwise orientation of @a p0, @a p1, and @a p2
	  */
	virtual void Update();

private:

	const Vector3* offset; ///< Vertex position offset

	const Matrix3* rotation; ///< Rotation matrix of object

	const Object* parent; ///< Pointer to the parent object
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_POLYGON_HPP
