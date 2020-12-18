#ifndef RAY_HPP
#define RAY_HPP

#include "vector3.hpp"

class ray{
public:
	vector3 pos; ///< The origin position of the ray
	vector3 dir; ///< The direction the ray is pointing

	/** Default constructor
	  */
	ray() : pos(), dir() { }

	/** Position and direction constructor
	  */	
	ray(const vector3 &p_, const vector3 &d_) : pos(p_), dir(d_.normalize()) { }

	/** Get the originating point of the ray
	  */
	vector3 getPosition() const { return pos; }
	
	/** Get the direction of the ray
	  */
	vector3 getDirection() const { return dir; }

	/** Set the originating point of the ray
	  */
	void setPosition(const vector3 &position){ pos = position; }

	/** Set the direction of the ray
	  */	
	void setDirection(const vector3 &direction){ dir = direction; }

	/** Get a vector from the origin of the ray pointing in its direction with a length @a t
	  */
	vector3 extend(const float &t){ return (pos+dir*t); }

	/** Move the origin point of the ray a distance @a dist along the direction it is pointing
	  */
	void move(const float &dist){ pos += dir*dist; }

	/** Dump information about the ray to stdout
	  */
	void dump() const ;
};

#endif
