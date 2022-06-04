#ifndef GRAPHICS_GEOMETRY_OTT_RAY_HPP
#define GRAPHICS_GEOMETRY_OTT_RAY_HPP

#include <math/OTTVector.hpp>

namespace ott {

struct Ray_t {
	Vector3 pos; ///< The origin position of the ray
	Vector3 dir; ///< The direction the ray is pointing

	/** Default constructor
	  */
	Ray_t() : 
		pos(), 
		dir() 
	{ 
	}

	/** Position and direction constructor
	  */	
	Ray_t(const Vector3 &p_, const Vector3 &d_) :
		pos(p_), 
		dir(d_.Normalize()) 
	{ 
	}

	/** Get a vector from the origin of the ray pointing in its direction with a length @a t
	  */
	Vector3 Extend(const float &t) const { 
		
		return (pos + dir * t); 
	}

	/** Move the origin point of the ray a distance @a dist along the direction it is pointing
	  */
	void Move(const float &dist){ 
		
		pos += dir * dist; 
	}
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_RAY_HPP
