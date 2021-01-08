#ifndef RAY_HPP
#define RAY_HPP

#include "Vector.hpp"

class ray{
public:
	Vector3 pos; ///< The origin position of the ray
	Vector3 dir; ///< The direction the ray is pointing

	/** Default constructor
	  */
	ray() : pos(), dir() { }

	/** Position and direction constructor
	  */	
	ray(const Vector3 &p_, const Vector3 &d_) : pos(p_), dir(d_.normalize()) { }

	/** Get the originating point of the ray
	  */
	Vector3 getPosition() const { return pos; }
	
	/** Get the direction of the ray
	  */
	Vector3 getDirection() const { return dir; }

	/** Set the originating point of the ray
	  */
	void setPosition(const Vector3 &position){ pos = position; }

	/** Set the direction of the ray
	  */	
	void setDirection(const Vector3 &direction){ dir = direction; }

	/** Get a vector from the origin of the ray pointing in its direction with a length @a t
	  */
	Vector3 extend(const float &t) const { return (pos+dir*t); }

	/** Move the origin point of the ray a distance @a dist along the direction it is pointing
	  */
	void move(const float &dist){ pos += dir*dist; }
};

#endif
