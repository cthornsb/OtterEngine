#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "matrix3.hpp"
#include "plane.hpp"

class triangle : public plane {
public:
	vector3 p0; /** The infinite plane which contains all three points
	              * This is redundant, because it stores one of the points, but it's fine for now.
	              */
	vector3 p1;
	vector3 p2;

	triangle() : plane(), p0(), p1(), p2() { }
	
	triangle(const vector3 &p0_, const vector3 &p1_, const vector3 &p2_) : plane(p0_, p1_, p2_), p0(p0_), p1(p1_), p2(p2_) { }

	void dump() const ;
	
	void transform(const matrix3 &rot);
};

#endif
