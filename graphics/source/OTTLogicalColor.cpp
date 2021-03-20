#include <cmath>

#include "OTTLogicalColor.hpp"

void OTTLogicalColor::invert() const {
	pArray[0] = 255 - pArray[0];
	pArray[1] = 255 - pArray[1];
	pArray[2] = 255 - pArray[2];
}

void OTTLogicalColor::toGrayscale(){
	// Based on the sRGB convention
	pArray[0] = clampUChar(pArray[0] / 255.0f * 0.2126f + pArray[1] / 255.0f * 0.7152f + pArray[2] / 255.0f * 0.0722f);
	pArray[1] = pArray[0];
	pArray[2] = pArray[0];
}

bool OTTLogicalColor::operator == (const ColorRGB& rhs) const {
	return ((pArray[0] == rhs.r) && (pArray[1] == rhs.g) && (pArray[2] == rhs.b));
}

ColorRGB OTTLogicalColor::operator + (const ColorRGB& rhs) const {
	return ColorRGB(
		clampFloat(pArray[0] + rhs.r),
		clampFloat(pArray[1] + rhs.g),
		clampFloat(pArray[2] + rhs.b)
	);
}

ColorRGB OTTLogicalColor::operator - (const ColorRGB& rhs) const {
	return ColorRGB(
		clampFloat(pArray[0] - rhs.r),
		clampFloat(pArray[1] - rhs.g),
		clampFloat(pArray[2] - rhs.b)
	);
}

ColorRGB OTTLogicalColor::operator * (const float& rhs) const {
	return ColorRGB(
		clampFloat(rhs * pArray[0]),
		clampFloat(rhs * pArray[1]),
		clampFloat(rhs * pArray[2])
	);
}

ColorRGB OTTLogicalColor::operator / (const float& rhs) const {
	return ColorRGB(
		clampFloat((1 / rhs) * pArray[0]),
		clampFloat((1 / rhs) * pArray[1]),
		clampFloat((1 / rhs) * pArray[2])
	);
}

void OTTLogicalColor::operator += (const ColorRGB& rhs){
	pArray[0] = clampUChar(getR() + rhs.r * rhs.a);
	pArray[1] = clampUChar(getG() + rhs.g * rhs.a);
	pArray[2] = clampUChar(getB() + rhs.b * rhs.a);
	pArray[3] = 255;
}

void OTTLogicalColor::operator -= (const ColorRGB& rhs){
	pArray[0] = clampUChar(getR() - rhs.r * rhs.a);
	pArray[1] = clampUChar(getG() - rhs.g * rhs.a);
	pArray[2] = clampUChar(getB() - rhs.b * rhs.a);
	pArray[3] = 255;
}

void OTTLogicalColor::operator *= (const ColorRGB& rhs){
	pArray[0] = clampUChar(getR() * rhs.r * rhs.a);
	pArray[1] = clampUChar(getG() * rhs.g * rhs.a);
	pArray[2] = clampUChar(getB() * rhs.b * rhs.a);
	pArray[3] = 255;
}

void OTTLogicalColor::operator /= (const ColorRGB& rhs){
	pArray[0] = clampUChar(getR() / rhs.r * rhs.a);
	pArray[1] = clampUChar(getG() / rhs.g * rhs.a);
	pArray[2] = clampUChar(getB() / rhs.b * rhs.a);
	pArray[3] = 255;
}

void OTTLogicalColor::setColor(const ColorRGB& color) {
	pArray[0] = color.r;
	pArray[1] = color.g;
	pArray[2] = color.b;
	pArray[3] = color.a;
}

void OTTLogicalColor::setColor(const float& r, const float& g, const float& b, const float& a/*=1*/) {
	pArray[0] = clampUChar(r);
	pArray[1] = clampUChar(g);
	pArray[2] = clampUChar(b);
	pArray[3] = clampUChar(a);
}

void OTTLogicalColor::lighten(const ColorRGB& color) {
	pArray[0] = clampUChar(std::fmaxf(getR(), color.r * color.a));
	pArray[1] = clampUChar(std::fmaxf(getG(), color.g * color.a));
	pArray[2] = clampUChar(std::fmaxf(getB(), color.b * color.a));
	pArray[3] = 255;
}

void OTTLogicalColor::darken(const ColorRGB& color) {
	pArray[0] = clampUChar(std::fminf(getR(), color.r * color.a));
	pArray[1] = clampUChar(std::fminf(getG(), color.g * color.a));
	pArray[2] = clampUChar(std::fminf(getB(), color.b * color.a));
	pArray[3] = 255;
}

void OTTLogicalColor::difference(const ColorRGB& color) {
	pArray[0] = clampUChar(std::abs(getR() - color.r * color.a));
	pArray[1] = clampUChar(std::abs(getG() - color.g * color.a));
	pArray[2] = clampUChar(std::abs(getB() - color.b * color.a));
	pArray[3] = 255;
}

bool OTTLogicalColor::compareColor(const ColorRGB& color, const float& margin/*=0.0f*/) {
	return ((std::abs(getR() - color.r) <= margin) && 
		(std::abs(getG() - color.g) <= margin) && 
		(std::abs(getB() - color.b) <= margin));
}

void OTTLogicalColor::dump() const {
}

void OTTLogicalColor::reset() {
	for (size_t i = 0; i < 4; i++)
		pArray[i] = 0;
}

unsigned char OTTLogicalColor::clampUChar(const float& value) {
	if (value > 1.f)
		return 255;
	else if (value < 0.f)
		return 0;
	return (unsigned char)(value * 255);
}

float OTTLogicalColor::clampFloat(const float& value) {
	if (value > 255.f)
		return 1.f;
	else if (value < 0.f)
		return 0.f;
	return (value / 255.f);
}

