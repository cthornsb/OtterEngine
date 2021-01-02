#include <iostream>
#include <cmath>

#include "vector2.hpp"

float vector2::operator [] (const unsigned int& index) const {
	if (index == 0)
		return x;
	else if (index == 1)
		return y;
	return NAN;
}

vector2& vector2::operator = (const vector2 &rhs){
	x = rhs.x;
	y = rhs.y;
	return (*this);
}

vector2& vector2::operator *= (const float &rhs){
	x *= rhs;
	y *= rhs;
	return (*this);
}

vector2& vector2::operator /= (const float &rhs){
	x /= rhs;
	y /= rhs;
	return (*this);
}

vector2& vector2::operator += (const vector2 &rhs){
	x += rhs.x;
	y += rhs.y;
	return (*this);
}

vector2& vector2::operator -= (const vector2 &rhs){
	x -= rhs.x;
	y -= rhs.y;
	return (*this);
}

vector2 vector2::normalize() const {
	float mag = length();
	return vector2(x/mag, y/mag);
}

vector2& vector2::normInPlace(){
	float mag = length();
	x /= mag;
	y /= mag;
	return (*this);
}

float vector2::length() const { 
	return std::sqrt(square()); 
}

float vector2::distance(const vector2& other) const {
	return (other - (*this)).length();
}

float vector2::angle(const vector2& other) const {
	return std::acos(cosTheta(other));
}

float vector2::cosTheta(const vector2& other) const {
	return ((*this) * other) / (this->length() * other.length());
}

void vector2::invert() {
	x *= -1;
	y *= -1;
}

void vector2::zero(){
	x = 0;
	y = 0;
}

void vector2::dump() const {
	std::cout << "x=" << x << ", y=" << y << std::endl;
}
