#ifndef PLANE_HPP
#define PLANE_HPP

#include "Vector.hpp"
#include "ray.hpp"

class plane{
public:
	/** Default constructor
	  */
	plane() : p(), norm(0, 0, 1) { }

	/** Position and normal constructor
	  */
	plane(const Vector3 &p_, const Vector3 &n_) : 
		p(p_), 
		norm(n_) 
	{ 
	}

	/** Constructor taking three points which lie on the surface of the plane
	  */
	plane(const Vector3 &p0_, const Vector3 &p1_, const Vector3 &p2_);

	/** Permanently offset the plane by an input vector
	  */
	void offsetPosition(const Vector3& vec) { p += vec; }

	/** Check if a ray intersects with the infinite plane defined by this object
	  * @param r The ray to check for intersection
	  * @return True if the plane is in front of the ray, and return false if the plane is behind the ray (i.e. they do not intersect)
	  */
	bool intersects(const ray& r) const ;

	/** Check if a ray intersects with the infinite plane defined by this object
	  * @param r The ray to check for intersection
	  * @param t The "distance" from the ray's vertex point, along its direction, to the point of intersection on the plane
	  * @return True if the plane is in front of the ray, and return false if the plane is behind the ray (i.e. they do not intersect)
	  */
	bool intersects(const ray& r, float& t) const ;

protected:
	Vector3 p; ///< A point on the surface of the plane

	Vector3 norm; ///< The normal vector to the surface of the plane
};

#endif
