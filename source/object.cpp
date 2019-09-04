#include "object.hpp"

void object::rotate(const double &theta, const double &phi, const double &psi){
	rot.setRotation(theta, phi, psi);
	for(size_t i = 0; i < polys.size(); i++){
		polys[i].transform(rot);
	}
}
