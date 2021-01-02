#include <cmath>

#include "LogicalColor.hpp"

unsigned char limitValueUChar(const float& value) {
	if (value > 1.f)
		return 255;
	else if (value < 0.f)
		return 0;
	return (unsigned char)(value * 255);
}

float limitValueFloat(const float& value) {
	if (value > 1.f)
		return 1.f;
	else if (value < 0.f)
		return 0.f;
	return value;
}

void LogicalColor::invert() const {
	pArray[0] = 255 - pArray[0];
	pArray[1] = 255 - pArray[1];
	pArray[2] = 255 - pArray[2];
}

void LogicalColor::toGrayscale(){
	// Based on the sRGB convention
	pArray[0] = limitValueUChar(pArray[0] / 255.0f * 0.2126f + pArray[1] / 255.0f * 0.7152f + pArray[2] / 255.0f * 0.0722f);
	pArray[1] = pArray[0];
	pArray[2] = pArray[0];
}

bool LogicalColor::operator == (const ColorRGB& rhs) const {
	return ((pArray[0] / 255.f == rhs.r) && (pArray[1] / 255.f == rhs.g) && (pArray[2] / 255.f == rhs.b));
}

ColorRGB LogicalColor::operator + (const ColorRGB& rhs) const {
	return ColorRGB(limitValueFloat(pArray[0] / 255.0f + rhs.r),
		limitValueFloat(pArray[1] / 255.0f + rhs.g),
		limitValueFloat(pArray[2] / 255.0f + rhs.b));
}

ColorRGB LogicalColor::operator - (const ColorRGB& rhs) const {
	return ColorRGB(limitValueFloat(pArray[0] / 255.0f - rhs.r),
		limitValueFloat(pArray[1] / 255.0f - rhs.g),
		limitValueFloat(pArray[2] / 255.0f - rhs.b));
}

ColorRGB LogicalColor::operator * (const float& rhs) const {
	return ColorRGB(limitValueFloat(rhs * pArray[0] / 255.0f),
		limitValueFloat(rhs * pArray[1] / 255.0f),
		limitValueFloat(rhs * pArray[2] / 255.0f));
}

ColorRGB LogicalColor::operator / (const float& rhs) const {
	return ColorRGB(limitValueFloat((1 / rhs) * pArray[0] / 255.0f),
		limitValueFloat((1 / rhs) * pArray[1] / 255.0f),
		limitValueFloat((1 / rhs) * pArray[2] / 255.0f));
}

void LogicalColor::operator += (const ColorRGB& rhs){
	pArray[0] = limitValueUChar(getR() + rhs.r * rhs.a);
	pArray[1] = limitValueUChar(getG() + rhs.g * rhs.a);
	pArray[2] = limitValueUChar(getB() + rhs.b * rhs.a);
	pArray[3] = 255;
}

void LogicalColor::operator -= (const ColorRGB& rhs){
	pArray[0] = limitValueUChar(getR() - rhs.r * rhs.a);
	pArray[1] = limitValueUChar(getG() - rhs.g * rhs.a);
	pArray[2] = limitValueUChar(getB() - rhs.b * rhs.a);
	pArray[3] = 255;
}

void LogicalColor::operator *= (const ColorRGB& rhs){
	pArray[0] = limitValueUChar(getR() * rhs.r * rhs.a);
	pArray[1] = limitValueUChar(getG() * rhs.g * rhs.a);
	pArray[2] = limitValueUChar(getB() * rhs.b * rhs.a);
	pArray[3] = 255;
}

void LogicalColor::operator /= (const ColorRGB& rhs){
	pArray[0] = limitValueUChar(getR() / rhs.r * rhs.a);
	pArray[1] = limitValueUChar(getG() / rhs.g * rhs.a);
	pArray[2] = limitValueUChar(getB() / rhs.b * rhs.a);
	pArray[3] = 255;
}

void LogicalColor::setColor(const ColorRGB& color) {
	pArray[0] = limitValueUChar(color.r);
	pArray[1] = limitValueUChar(color.g);
	pArray[2] = limitValueUChar(color.b);
	pArray[3] = limitValueUChar(color.a);
}

void LogicalColor::setColor(const float& r, const float& g, const float& b, const float& a/*=1*/) {
	pArray[0] = limitValueUChar(r);
	pArray[1] = limitValueUChar(g);
	pArray[2] = limitValueUChar(b);
	pArray[3] = limitValueUChar(a);
}

void LogicalColor::lighten(const ColorRGB& color) {
	pArray[0] = limitValueUChar(std::fmaxf(getR(), color.r * color.a));
	pArray[1] = limitValueUChar(std::fmaxf(getG(), color.g * color.a));
	pArray[2] = limitValueUChar(std::fmaxf(getB(), color.b * color.a));
	pArray[3] = 255;
}

void LogicalColor::darken(const ColorRGB& color) {
	pArray[0] = limitValueUChar(std::fminf(getR(), color.r * color.a));
	pArray[1] = limitValueUChar(std::fminf(getG(), color.g * color.a));
	pArray[2] = limitValueUChar(std::fminf(getB(), color.b * color.a));
	pArray[3] = 255;
}

void LogicalColor::difference(const ColorRGB& color) {
	pArray[0] = limitValueUChar(std::abs(getR() - color.r * color.a));
	pArray[1] = limitValueUChar(std::abs(getG() - color.g * color.a));
	pArray[2] = limitValueUChar(std::abs(getB() - color.b * color.a));
	pArray[3] = 255;
}

bool LogicalColor::compareColor(const ColorRGB& color, const float& margin/*=0.0f*/) {
	return ((std::abs(getR() - color.r) <= margin) && 
		(std::abs(getG() - color.g) <= margin) && 
		(std::abs(getB() - color.b) <= margin));
}

void LogicalColor::dump() const {
}

void LogicalColor::reset() {
	for (size_t i = 0; i < 4; i++)
		pArray[i] = 0;
}
