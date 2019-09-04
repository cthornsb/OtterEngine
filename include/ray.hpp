#ifndef RAY_HPP
#define RAY_HPP

#include "vector3.hpp"

class ray{
public:
	vector3 pos;
	vector3 dir;

	ray() : pos(), dir() { }
	
	ray(const vector3 &p_, const vector3 &d_) : pos(p_), dir(d_.normalize()) { }

	vector3 extend(const double &t){ return (pos+dir*t); }

	void move(const double &dist){ pos += dir*dist; }

	void dump() const ;
};

#endif
