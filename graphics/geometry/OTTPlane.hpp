#ifndef GRAPHICS_GEOMETRY_OTT_PLANE_HPP
#define GRAPHICS_GEOMETRY_OTT_PLANE_HPP

#include "OTTRay.hpp"

#include <math/OTTVector.hpp>

namespace ott {

class Plane{
public:
	/** Default constructor
	  */
	Plane() : pos(), norm(0, 0, 1) { }

	/** Position and normal constructor
	  */
	Plane(const Vector3 &p_, const Vector3 &n_) :
		pos(p_),
		norm(n_) 
	{ 
	}

	/** Constructor taking three points which lie on the surface of the plane
	  */
	Plane(const Vector3 &p0_, const Vector3 &p1_, const Vector3 &p2_);

	/** Permanently offset the plane by an input vector
	  */
	void OffsetPosition(const Vector3& vec) { 
		
		pos += vec; 
	}

	/** Check if a ray intersects with the infinite plane defined by this object
	  * @param r The ray to check for intersection
	  * @return True if the plane is in front of the ray, and return false if the plane is behind the ray (i.e. they do not intersect)
	  */
	bool Intersects(const Ray_t& r) const ;

	/** Check if a ray intersects with the infinite plane defined by this object
	  * @param r The ray to check for intersection
	  * @param t The "distance" from the ray's vertex point, along its direction, to the point of intersection on the plane
	  * @return True if the plane is in front of the ray, and return false if the plane is behind the ray (i.e. they do not intersect)
	  */
	bool Intersects(const Ray_t& r, float& t) const ;

protected:
	Vector3 pos; ///< A point on the surface of the plane

	Vector3 norm; ///< The normal vector to the surface of the plane
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_PLANE_HPP
