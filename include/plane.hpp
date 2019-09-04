#ifndef PLANE_HPP
#define PLANE_HPP

#include "vector3.hpp"
#include "ray.hpp"

class plane{
public:
	vector3 p; ///< A point on the surface of the plane
	vector3 norm; ///< The normal vector to the surface of the plane

	/** Default constructor
	  */
	plane() : p(), norm(0, 0, 1) { }

	/** Position and normal constructor
	  */
	plane(const vector3 &p_, const vector3 &n_) : p(p_), norm(n_) { }

	/** 
	  */	
	plane(const vector3 &p0_, const vector3 &p1_, const vector3 &p2_);

	bool intersects(const ray &r) const ;

	bool intersects(const ray &r, double &t) const ;

	void dump() const ;
};

#endif
