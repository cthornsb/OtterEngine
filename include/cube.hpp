#ifndef CUBE_HPP
#define CUBE_HPP

#include "object.hpp"

class cube : public object {
public:
	cube(const vector3 &pos_, const double &X, const double &Y, const double &Z);
	
	void build();
	
private:
	double hX;
	double hY;
	double hZ;
};

#endif
