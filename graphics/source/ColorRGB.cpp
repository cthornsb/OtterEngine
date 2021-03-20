#include <iostream>

#include "ColorRGB.hpp"

ColorRGB::ColorRGB(const float& value, const float& alpha/*=1.f*/) :
	r(toUChar(value)),
	g(toUChar(value)),
	b(toUChar(value)),
	a(toUChar(alpha))
{
}

ColorRGB::ColorRGB(const float& red, const float& green, const float& blue, const float& alpha/*=1.f*/) :
	r(toUChar(red)),
	g(toUChar(green)),
	b(toUChar(blue)),
	a(toUChar(alpha))
{
}

ColorRGB ColorRGB::invert() const {
	return ColorRGB(255 - r, 255 - g, 255 - b);
}

unsigned char& ColorRGB::operator [] (const unsigned char& index){
	static unsigned char dummy = 0;
	switch(index){
	case 0: // red
		return r;
	case 1: // green
		return g;
	case 2: // blue
		return b;
	case 3: // alpha
		return a;
	default:
		break;
	}
	return dummy;
}

unsigned char ColorRGB::operator [] (const unsigned char& index) const {
	switch(index){
	case 0: // red
		return r;
	case 1: // green
		return g;
	case 2: // blue
		return b;
	case 3: // alpha
		return a;
	default:
		break;
	}
	return 0;
}

void ColorRGB::toGrayscale(){
	// Based on the sRGB convention
	float value = toFloat(r) * 0.2126f + toFloat(g) * 0.7152f + toFloat(b) * 0.0722f;
	r = toUChar(value);
	g = toUChar(value);
	b = toUChar(value);
}

ColorRGB ColorRGB::operator + (const ColorRGB &rhs) const {
	float rprime = (r + rhs.r)/255.0f;
	float gprime = (g + rhs.g)/255.0f;
	float bprime = (b + rhs.b)/255.0f;
	return ColorRGB((rprime <= 1 ? rprime : 1), (gprime <= 1 ? gprime : 1), (bprime <= 1 ? bprime : 1));
}

ColorRGB ColorRGB::operator - (const ColorRGB &rhs) const {
	float rprime = (r - rhs.r)/255.0f;
	float gprime = (g - rhs.g)/255.0f;
	float bprime = (b - rhs.b)/255.0f;
	return ColorRGB((rprime > 0 ? rprime : 0), (gprime > 1 ? gprime : 0), (bprime > 1 ? bprime : 0));
}

ColorRGB ColorRGB::operator * (const float& rhs) const {
	return ColorRGB(r*rhs, g*rhs, b*rhs);
}

ColorRGB ColorRGB::operator / (const float& rhs) const {
	return ColorRGB(r/rhs, g/rhs, b/rhs);
}

ColorRGB& ColorRGB::operator += (const ColorRGB &rhs){
	return ((*this) = (*this) + rhs);
}

ColorRGB& ColorRGB::operator -= (const ColorRGB &rhs){
	return ((*this) = (*this) - rhs);
}

ColorRGB& ColorRGB::operator *= (const float& rhs){
	return ((*this) = (*this) * rhs);
}

ColorRGB& ColorRGB::operator /= (const float& rhs){
	return ((*this) = (*this) / rhs);
}

void ColorRGB::dump() const {
	std::cout << "r=" << toFloat(r) << ", g=" << toFloat(g) << ", b=" << toFloat(b) << std::endl;
}

ColorRGB ColorRGB::heatMap(const float& x, const float& xmax, const float& xmin/*=0*/) {
	float rgb;
	if (x < xmin)
		rgb = 0.f;
	else if (x > xmax)
		rgb = 1.f;
	else
		rgb = 1.0f - x / (xmax - xmin);
	return ColorRGB(rgb, rgb, rgb);
}

