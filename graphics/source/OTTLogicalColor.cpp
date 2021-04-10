#include <cmath>

#include "OTTLogicalColor.hpp"

float getComponentWithOpacity(const unsigned char& comp, const unsigned char& opacity) {
	return (1.f - opacity * (255 - comp) / 65025.f);
}

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
		clampFloat(1.f * pArray[0] + rhs.r),
		clampFloat(1.f * pArray[1] + rhs.g),
		clampFloat(1.f * pArray[2] + rhs.b)
	);
}

ColorRGB OTTLogicalColor::operator - (const ColorRGB& rhs) const {
	return ColorRGB(
		clampFloat(1.f * pArray[0] - rhs.r),
		clampFloat(1.f * pArray[1] - rhs.g),
		clampFloat(1.f * pArray[2] - rhs.b)
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
	pArray[0] = clampUChar(getR() + getComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = clampUChar(getG() + getComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = clampUChar(getB() + getComponentWithOpacity(rhs.b, rhs.a));
	setAlpha(1.f);
}

void OTTLogicalColor::operator -= (const ColorRGB& rhs){
	pArray[0] = clampUChar(getR() - getComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = clampUChar(getG() - getComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = clampUChar(getB() - getComponentWithOpacity(rhs.b, rhs.a));
	setAlpha(1.f);
}

void OTTLogicalColor::operator *= (const ColorRGB& rhs){
	pArray[0] = clampUChar(getR() * getComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = clampUChar(getG() * getComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = clampUChar(getB() * getComponentWithOpacity(rhs.b, rhs.a));
	setAlpha(1.f);
}

void OTTLogicalColor::operator /= (const ColorRGB& rhs){
	pArray[0] = clampUChar(getR() / getComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = clampUChar(getG() / getComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = clampUChar(getB() / getComponentWithOpacity(rhs.b, rhs.a));
	setAlpha(1.f);
}

void OTTLogicalColor::operator = (const ColorRGB& rhs){
	pArray[0] = rhs.r;
	pArray[1] = rhs.g;
	pArray[2] = rhs.b;
	setAlpha(rhs.a);
}

void OTTLogicalColor::operator = (const OTTLogicalColor& rhs){
	for(int i = 0; i < 3; i++)
		pArray[i] = rhs[i];
	setAlpha(rhs.getA()); // Safely set and get the alpha values (in case they don't exist)
}

void OTTLogicalColor::setColor(const ColorRGB& color) {
	pArray[0] = color.r;
	pArray[1] = color.g;
	pArray[2] = color.b;
	setAlpha(color.a);
}

void OTTLogicalColor::setColor(const OTTLogicalColor& color){
	for(int i = 0; i < 3; i++)
		pArray[i] = color[i];
	setAlpha(color.getA()); // Safely set and get the alpha values (in case they don't exist)
}

void OTTLogicalColor::setColor(const unsigned char* arr){
	for(int i = 0; i < (bAlpha ? 4 : 3); i++)
		pArray[i] = arr[i];
}

void OTTLogicalColor::setColor(const float& r, const float& g, const float& b, const float& a/*=1*/) {
	pArray[0] = clampUChar(r);
	pArray[1] = clampUChar(g);
	pArray[2] = clampUChar(b);
	setAlpha(clampUChar(a));
}

void OTTLogicalColor::lighten(const ColorRGB& color) {
	pArray[0] = clampUChar(std::fmaxf(getR(), getComponentWithOpacity(color.r, color.a)));
	pArray[1] = clampUChar(std::fmaxf(getG(), getComponentWithOpacity(color.g, color.a)));
	pArray[2] = clampUChar(std::fmaxf(getB(), getComponentWithOpacity(color.b, color.a)));
	setAlpha(1.f);
}

void OTTLogicalColor::darken(const ColorRGB& color) {
	pArray[0] = clampUChar(std::fminf(getR(), getComponentWithOpacity(color.r, color.a)));
	pArray[1] = clampUChar(std::fminf(getG(), getComponentWithOpacity(color.g, color.a)));
	pArray[2] = clampUChar(std::fminf(getB(), getComponentWithOpacity(color.b, color.a)));
	setAlpha(1.f);
}

void OTTLogicalColor::difference(const ColorRGB& color) {
	pArray[0] = clampUChar(std::abs(getR() - getComponentWithOpacity(color.r, color.a)));
	pArray[1] = clampUChar(std::abs(getG() - getComponentWithOpacity(color.g, color.a)));
	pArray[2] = clampUChar(std::abs(getB() - getComponentWithOpacity(color.b, color.a)));
	setAlpha(1.f);
}

bool OTTLogicalColor::compareColor(const ColorRGB& color, const float& margin/*=0.0f*/) {
	return ((std::abs(getR() - color.r) <= margin) && 
		(std::abs(getG() - color.g) <= margin) && 
		(std::abs(getB() - color.b) <= margin));
}

void OTTLogicalColor::dump() const {
}

void OTTLogicalColor::reset() {
	for (size_t i = 0; i < (bAlpha ? 4 : 3); i++)
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

