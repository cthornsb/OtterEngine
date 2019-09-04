#include <iostream>
#include <cmath>

#include "vector3.hpp"

vector3& vector3::operator = (const vector3 &rhs){
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return (*this);
}

vector3& vector3::operator *= (const double &rhs){
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return (*this);
}

vector3& vector3::operator /= (const double &rhs){
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return (*this);
}

vector3& vector3::operator += (const vector3 &rhs){
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return (*this);
}

vector3& vector3::operator -= (const vector3 &rhs){
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return (*this);
}

vector3 vector3::cross(const vector3 &rhs) const {
	return vector3((y*rhs.z - rhs.y*z),
	               (rhs.x*z - x*rhs.z),
	               (x*rhs.y - rhs.x*y));
}

vector3 vector3::normalize() const {
	double mag = length();
	return vector3(x/mag, y/mag, z/mag);
}

vector3& vector3::normInPlace(){
	double mag = length();
	x /= mag;
	y /= mag;
	z /= mag;
	return (*this);
}

double vector3::length() const { 
	return std::sqrt(square()); 
}

void vector3::zero(){
	x = 0;
	y = 0;
	z = 0;
}

void vector3::dump() const {
	std::cout << "x=" << x << ", y=" << y << ", z=" << z << std::endl;
}
