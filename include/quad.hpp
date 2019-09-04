#ifndef QUAD_HPP
#define QUAD_HPP

class quad : public triangle {
public:
	/** Default constructor
	  */
	quad() : triangle(), p3() { }
	
	/** Constructor from four points
	  */
	quad(const vector3 &p0_, const vector3 &p1_, const vector3 &p2_, const vector3 &p3_) : triangle(p0_, p1_, p2_), p3(p3_) { }

	/** Constructor from a triangle and a 4th point
	  * @note This is included for completeness, as it is probably not very useful
	  */
	quad(const triangle &tri, const vector3 &p3_) : triangle(tri), p3(p3_) { }

private:
	vector3 p3;
};

#endif
