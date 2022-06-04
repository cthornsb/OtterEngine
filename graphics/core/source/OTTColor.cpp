#include "OTTColor.hpp"

#include <iostream>

ott::ColorRGB::ColorRGB(const float& value, const float& alpha/*=1.f*/) :
	r(this->ToUChar(value)),
	g(this->ToUChar(value)),
	b(this->ToUChar(value)),
	a(this->ToUChar(alpha))
{
}

ott::ColorRGB::ColorRGB(const float& red, const float& green, const float& blue, const float& alpha/*=1.f*/) :
	r(this->ToUChar(red)),
	g(this->ToUChar(green)),
	b(this->ToUChar(blue)),
	a(this->ToUChar(alpha))
{
}

ott::ColorRGB ott::ColorRGB::Invert() const {
	return ColorRGB((255 - r) / 255.f, (255 - g) / 255.f, (255 - b) / 255.f);
}

unsigned char& ott::ColorRGB::operator [] (const unsigned char& index){
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

unsigned char ott::ColorRGB::operator [] (const unsigned char& index) const {
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

void ott::ColorRGB::ToGrayscale(){
	// Based on the sRGB convention
	float value = this->ToFloat(r) * 0.2126f + this->ToFloat(g) * 0.7152f + this->ToFloat(b) * 0.0722f;
	r = this->ToUChar(value);
	g = this->ToUChar(value);
	b = this->ToUChar(value);
}

ott::ColorRGB ott::ColorRGB::operator + (const ColorRGB &rhs) const {
	float rprime = (r + rhs.r)/255.0f;
	float gprime = (g + rhs.g)/255.0f;
	float bprime = (b + rhs.b)/255.0f;
	return ColorRGB((rprime <= 1 ? rprime : 1), (gprime <= 1 ? gprime : 1), (bprime <= 1 ? bprime : 1));
}

ott::ColorRGB ott::ColorRGB::operator - (const ColorRGB &rhs) const {
	float rprime = (r - rhs.r)/255.0f;
	float gprime = (g - rhs.g)/255.0f;
	float bprime = (b - rhs.b)/255.0f;
	return ColorRGB((rprime > 0 ? rprime : 0), (gprime > 1 ? gprime : 0), (bprime > 1 ? bprime : 0));
}

ott::ColorRGB ott::ColorRGB::operator * (const float& rhs) const {
	return ColorRGB(r*rhs, g*rhs, b*rhs);
}

ott::ColorRGB ott::ColorRGB::operator / (const float& rhs) const {
	return ColorRGB(r/rhs, g/rhs, b/rhs);
}

ott::ColorRGB& ott::ColorRGB::operator += (const ColorRGB &rhs){
	return ((*this) = (*this) + rhs);
}

ott::ColorRGB& ott::ColorRGB::operator -= (const ColorRGB &rhs){
	return ((*this) = (*this) - rhs);
}

ott::ColorRGB& ott::ColorRGB::operator *= (const float& rhs){
	return ((*this) = (*this) * rhs);
}

ott::ColorRGB& ott::ColorRGB::operator /= (const float& rhs){
	return ((*this) = (*this) / rhs);
}

void ott::ColorRGB::Dump() const {
	std::cout << "r=" << this->ToFloat(r) << ", g=" << this->ToFloat(g) << ", b=" << this->ToFloat(b) << std::endl;
}

ott::ColorRGB ott::ColorRGB::HeatMap(const float& x, const float& xmax, const float& xmin/*=0*/) {
	float rgb;
	if (x < xmin)
		rgb = 0.f;
	else if (x > xmax)
		rgb = 1.f;
	else
		rgb = 1.0f - x / (xmax - xmin);
	return ColorRGB(rgb, rgb, rgb);
}

const ott::ColorRGB ott::colors::Black(0.f, 0.f, 0.f);
const ott::ColorRGB ott::colors::DkGray(2 / 3.0f, 2 / 3.0f, 2 / 3.0f);
const ott::ColorRGB ott::colors::LtGray(1 / 3.0f, 1 / 3.0f, 1 / 3.0f);
const ott::ColorRGB ott::colors::White(1.f, 1.f, 1.f);

// Primary colors
const ott::ColorRGB ott::colors::Red(1.f, 0.f, 0.f);
const ott::ColorRGB ott::colors::Green(0.f, 1.f, 0.f);
const ott::ColorRGB ott::colors::Blue(0.f, 0.f, 1.f);

// Secondary colors
const ott::ColorRGB ott::colors::Yellow(1.f, 1.f, 0.f);
const ott::ColorRGB ott::colors::Magenta(1.f, 0.f, 1.f);
const ott::ColorRGB ott::colors::Cyan(0.f, 1.f, 1.f);

// Tertiary colors
const ott::ColorRGB ott::colors::Orange(1.f, 0.5f, 0.f);
const ott::ColorRGB ott::colors::Chartreuse(0.5f, 1.f, 0.f);
const ott::ColorRGB ott::colors::Spring(0.f, 1.f, 0.5f);
const ott::ColorRGB ott::colors::Azure(0.f, 0.5f, 1.f);
const ott::ColorRGB ott::colors::Violet(0.5f, 0.f, 1.f);
const ott::ColorRGB ott::colors::Rose(1.f, 0.f, 0.5f);