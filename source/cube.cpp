#include "cube.hpp"

cube::cube(const vector3 &pos_, const double &X, const double &Y, const double &Z) : object(pos_), hX(X/2), hY(Y/2), hZ(Z/2) {
	build();
}

void cube::build(){
	vector3 v[8] = {vector3(hX, hY, hZ), vector3(hX, -hY, hZ), vector3(-hX, -hY, hZ), vector3(-hX, hY, hZ),
	                vector3(hX, hY, -hZ), vector3(hX, -hY, -hZ), vector3(-hX, -hY, -hZ), vector3(-hX, hY, -hZ)};
	polys.push_back(triangle(v[0], v[1], v[2]));
	polys.push_back(triangle(v[2], v[3], v[0]));
	polys.push_back(triangle(v[1], v[0], v[4]));
	polys.push_back(triangle(v[4], v[5], v[1]));
	polys.push_back(triangle(v[5], v[4], v[7]));
	polys.push_back(triangle(v[7], v[6], v[5]));
	polys.push_back(triangle(v[7], v[3], v[2]));
	polys.push_back(triangle(v[2], v[6], v[7]));
	polys.push_back(triangle(v[7], v[4], v[0]));
	polys.push_back(triangle(v[0], v[3], v[7]));
	polys.push_back(triangle(v[6], v[2], v[1]));
	polys.push_back(triangle(v[1], v[5], v[6]));
}
