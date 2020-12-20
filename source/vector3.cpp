#include <iostream>
#include <cmath>

#include "vector3.hpp"

const vector3 zeroVector(0, 0, 0);
const vector3 unitVectorX(1, 0, 0);
const vector3 unitVectorY(0, 1, 0);
const vector3 unitVectorZ(0, 0, 1);

float vector3::operator [] (const unsigned int& index) const {
	if (index == 0)
		return x;
	else if (index == 1)
		return y;
	else if (index == 2)
		return z;
	return NAN;
}

vector3& vector3::operator = (const vector3 &rhs){
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return (*this);
}

vector3& vector3::operator *= (const float &rhs){
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return (*this);
}

vector3& vector3::operator /= (const float &rhs){
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
	float mag = length();
	return vector3(x/mag, y/mag, z/mag);
}

vector3& vector3::normInPlace(){
	float mag = length();
	x /= mag;
	y /= mag;
	z /= mag;
	return (*this);
}

float vector3::length() const { 
	return std::sqrt(square()); 
}

float vector3::distance(const vector3& other) const {
	return (other - (*this)).length();
}

float vector3::angle(const vector3& other) const {
	return std::acos(cosTheta(other));
}

float vector3::cosTheta(const vector3& other) const {
	return ((*this) * other) / (this->length() * other.length());
}

void vector3::invert() {
	x *= -1;
	y *= -1;
	z *= -1;
}

void vector3::zero(){
	x = 0;
	y = 0;
	z = 0;
}

void vector3::dump() const {
	std::cout << "x=" << x << ", y=" << y << ", z=" << z << std::endl;
}
