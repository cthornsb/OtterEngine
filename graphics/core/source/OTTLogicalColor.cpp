#include "OTTLogicalColor.hpp"
#include "OTTColor.hpp"

#include <cmath>

void ott::LogicalColor::Invert() const {
	pArray[0] = 255 - pArray[0];
	pArray[1] = 255 - pArray[1];
	pArray[2] = 255 - pArray[2];
}

void ott::LogicalColor::ToGrayscale() {
	// Based on the sRGB convention
	pArray[0] = LogicalColor::ClampUChar(pArray[0] / 255.0f * 0.2126f + pArray[1] / 255.0f * 0.7152f + pArray[2] / 255.0f * 0.0722f);
	pArray[1] = pArray[0];
	pArray[2] = pArray[0];
}

bool ott::LogicalColor::operator == (const ColorRGB& rhs) const {
	return ((pArray[0] == rhs.r) && (pArray[1] == rhs.g) && (pArray[2] == rhs.b));
}

ott::ColorRGB ott::LogicalColor::operator + (const ColorRGB& rhs) const {
	return ColorRGB(
		LogicalColor::ClampFloat(1.f * pArray[0] + rhs.r),
		LogicalColor::ClampFloat(1.f * pArray[1] + rhs.g),
		LogicalColor::ClampFloat(1.f * pArray[2] + rhs.b)
	);
}

ott::ColorRGB ott::LogicalColor::operator - (const ColorRGB& rhs) const {
	return ColorRGB(
		LogicalColor::ClampFloat(1.f * pArray[0] - rhs.r),
		LogicalColor::ClampFloat(1.f * pArray[1] - rhs.g),
		LogicalColor::ClampFloat(1.f * pArray[2] - rhs.b)
	);
}

ott::ColorRGB ott::LogicalColor::operator * (const float& rhs) const {
	return ColorRGB(
		LogicalColor::ClampFloat(rhs * pArray[0]),
		LogicalColor::ClampFloat(rhs * pArray[1]),
		LogicalColor::ClampFloat(rhs * pArray[2])
	);
}

ott::ColorRGB ott::LogicalColor::operator / (const float& rhs) const {
	return ColorRGB(
		LogicalColor::ClampFloat((1 / rhs) * pArray[0]),
		LogicalColor::ClampFloat((1 / rhs) * pArray[1]),
		LogicalColor::ClampFloat((1 / rhs) * pArray[2])
	);
}

void ott::LogicalColor::operator += (const ColorRGB& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() + LogicalColor::GetComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = LogicalColor::ClampUChar(this->G() + LogicalColor::GetComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = LogicalColor::ClampUChar(this->B() + LogicalColor::GetComponentWithOpacity(rhs.b, rhs.a));
	this->SetA(1.f);
}

void ott::LogicalColor::operator -= (const ColorRGB& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() - LogicalColor::GetComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = LogicalColor::ClampUChar(this->G() - LogicalColor::GetComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = LogicalColor::ClampUChar(this->B() - LogicalColor::GetComponentWithOpacity(rhs.b, rhs.a));
	this->SetA(1.f);
}

void ott::LogicalColor::operator *= (const ColorRGB& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() * LogicalColor::GetComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = LogicalColor::ClampUChar(this->G() * LogicalColor::GetComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = LogicalColor::ClampUChar(this->B() * LogicalColor::GetComponentWithOpacity(rhs.b, rhs.a));
	this->SetA(1.f);
}

void ott::LogicalColor::operator /= (const ColorRGB& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() / LogicalColor::GetComponentWithOpacity(rhs.r, rhs.a));
	pArray[1] = LogicalColor::ClampUChar(this->G() / LogicalColor::GetComponentWithOpacity(rhs.g, rhs.a));
	pArray[2] = LogicalColor::ClampUChar(this->B() / LogicalColor::GetComponentWithOpacity(rhs.b, rhs.a));
	this->SetA(1.f);
}

void ott::LogicalColor::operator += (const LogicalColor& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() + rhs.R());
	pArray[1] = LogicalColor::ClampUChar(this->G() + rhs.G());
	pArray[2] = LogicalColor::ClampUChar(this->B() + rhs.B());
	this->SetA(1.f);
}

void ott::LogicalColor::operator -= (const LogicalColor& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() - rhs.R());
	pArray[1] = LogicalColor::ClampUChar(this->G() - rhs.G());
	pArray[2] = LogicalColor::ClampUChar(this->B() - rhs.B());
	this->SetA(1.f);
}

void ott::LogicalColor::operator *= (const LogicalColor& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() * rhs.R());
	pArray[1] = LogicalColor::ClampUChar(this->G() * rhs.G());
	pArray[2] = LogicalColor::ClampUChar(this->B() * rhs.B());
	this->SetA(1.f);
}

void ott::LogicalColor::operator /= (const LogicalColor& rhs) {
	pArray[0] = LogicalColor::ClampUChar(this->R() / rhs.R());
	pArray[1] = LogicalColor::ClampUChar(this->G() / rhs.G());
	pArray[2] = LogicalColor::ClampUChar(this->B() / rhs.B());
	this->SetA(1.f);
}

void ott::LogicalColor::operator = (const ColorRGB& rhs) {
	pArray[0] = rhs.r;
	pArray[1] = rhs.g;
	pArray[2] = rhs.b;
	this->SetA(rhs.a);
}

void ott::LogicalColor::operator = (const LogicalColor& rhs) {
	for (int i = 0; i < 3; i++)
		pArray[i] = rhs[i];
	this->SetA(rhs.A()); // Safely set and get the alpha values (in case they don't exist)
}

void ott::LogicalColor::Set(const ColorRGB& color) {
	pArray[0] = color.r;
	pArray[1] = color.g;
	pArray[2] = color.b;
	this->SetA(color.a);
}

void ott::LogicalColor::Set(const LogicalColor& color) {
	for (int i = 0; i < 3; i++)
		pArray[i] = color[i];
	this->SetA(color.A()); // Safely set and get the alpha values (in case they don't exist)
}

void ott::LogicalColor::Set(const unsigned char* arr) {
	for (int i = 0; i < (bAlpha ? 4 : 3); i++)
		pArray[i] = arr[i];
}

void ott::LogicalColor::Set(const float& r, const float& g, const float& b, const float& a/*=1*/) {
	pArray[0] = LogicalColor::ClampUChar(r);
	pArray[1] = LogicalColor::ClampUChar(g);
	pArray[2] = LogicalColor::ClampUChar(b);
	this->SetA(LogicalColor::ClampUChar(a));
}

void ott::LogicalColor::Lighten(const ColorRGB& color) {
	pArray[0] = LogicalColor::ClampUChar(std::fmaxf(this->R(), LogicalColor::GetComponentWithOpacity(color.r, color.a)));
	pArray[1] = LogicalColor::ClampUChar(std::fmaxf(this->G(), LogicalColor::GetComponentWithOpacity(color.g, color.a)));
	pArray[2] = LogicalColor::ClampUChar(std::fmaxf(this->B(), LogicalColor::GetComponentWithOpacity(color.b, color.a)));
	this->SetA(1.f);
}

void ott::LogicalColor::Darken(const ColorRGB& color) {
	pArray[0] = LogicalColor::ClampUChar(std::fminf(this->R(), LogicalColor::GetComponentWithOpacity(color.r, color.a)));
	pArray[1] = LogicalColor::ClampUChar(std::fminf(this->G(), LogicalColor::GetComponentWithOpacity(color.g, color.a)));
	pArray[2] = LogicalColor::ClampUChar(std::fminf(this->B(), LogicalColor::GetComponentWithOpacity(color.b, color.a)));
	this->SetA(1.f);
}

void ott::LogicalColor::Difference(const ColorRGB& color) {
	pArray[0] = LogicalColor::ClampUChar(std::abs(this->R() - LogicalColor::GetComponentWithOpacity(color.r, color.a)));
	pArray[1] = LogicalColor::ClampUChar(std::abs(this->G() - LogicalColor::GetComponentWithOpacity(color.g, color.a)));
	pArray[2] = LogicalColor::ClampUChar(std::abs(this->B() - LogicalColor::GetComponentWithOpacity(color.b, color.a)));
	this->SetA(1.f);
}

void ott::LogicalColor::Average(const ColorRGB& color) {
	auto getComponentAlpha = [](const unsigned char& prevComp, const unsigned char& nextComp, const float& alpha) {
		if (!nextComp) // Prevent division by zero
			return 0.f;
		return (nextComp * (alpha + ((float)prevComp / (float)nextComp) * (1.f - alpha)) / 255.f);
	};
	pArray[0] = LogicalColor::ClampUChar((this->R() + getComponentAlpha(pArray[0], color.r, color.a / 255.f)) / 2.f);
	pArray[1] = LogicalColor::ClampUChar((this->G() + getComponentAlpha(pArray[1], color.g, color.a / 255.f)) / 2.f);
	pArray[2] = LogicalColor::ClampUChar((this->B() + getComponentAlpha(pArray[2], color.b, color.a / 255.f)) / 2.f);
	this->SetA(1.f);
}

void ott::LogicalColor::Lighten(const LogicalColor& color) {
	pArray[0] = LogicalColor::ClampUChar(std::fmaxf(this->R(), color.R()));
	pArray[1] = LogicalColor::ClampUChar(std::fmaxf(this->G(), color.G()));
	pArray[2] = LogicalColor::ClampUChar(std::fmaxf(this->B(), color.B()));
	this->SetA(1.f);
}

void ott::LogicalColor::Darken(const LogicalColor& color) {
	pArray[0] = LogicalColor::ClampUChar(std::fminf(this->R(), color.R()));
	pArray[1] = LogicalColor::ClampUChar(std::fminf(this->G(), color.G()));
	pArray[2] = LogicalColor::ClampUChar(std::fminf(this->B(), color.B()));
	this->SetA(1.f);
}

void ott::LogicalColor::Difference(const LogicalColor& color) {
	pArray[0] = LogicalColor::ClampUChar(std::abs(this->R() - color.R()));
	pArray[1] = LogicalColor::ClampUChar(std::abs(this->G() - color.G()));
	pArray[2] = LogicalColor::ClampUChar(std::abs(this->B() - color.B()));
	this->SetA(1.f);
}

void ott::LogicalColor::Average(const LogicalColor& color) {
	auto getComponentAlpha = [](const unsigned char& prevComp, const unsigned char& nextComp, const float& alpha) {
		if (!nextComp) // Prevent division by zero
			return 0.f;
		return (nextComp * (alpha + ((float)prevComp / (float)nextComp) * (1.f - alpha)) / 255.f);
	};
	pArray[0] = LogicalColor::ClampUChar((this->R() + color.R()) / 2.f);
	pArray[1] = LogicalColor::ClampUChar((this->G() + color.G()) / 2.f);
	pArray[2] = LogicalColor::ClampUChar((this->B() + color.B()) / 2.f);
	this->SetA(1.f);
}

bool ott::LogicalColor::CompareColor(const ColorRGB& color, const float& margin/*=0.0f*/) {
	return (
		(std::abs(this->R() - color.r) <= margin) &&
		(std::abs(this->G() - color.g) <= margin) &&
		(std::abs(this->B() - color.b) <= margin)
		);
}

void ott::LogicalColor::Dump() const {
}

void ott::LogicalColor::Reset() {
	for (size_t i = 0; i < (bAlpha ? 4 : 3); i++)
		pArray[i] = 0;
}

unsigned char ott::LogicalColor::LogicalColor::ClampUChar(const float& value) {
	if (value > 1.f)
		return 255;
	else if (value < 0.f)
		return 0;
	return (unsigned char)(value * 255.f);
}

float ott::LogicalColor::LogicalColor::ClampFloat(const float& value) {
	if (value > 255.f)
		return 1.f;
	else if (value < 0.f)
		return 0.f;
	return (value / 255.f);
}

float ott::LogicalColor::LogicalColor::GetComponentWithOpacity(const unsigned char& comp, const unsigned char& opacity) {
	return (comp * opacity / 65025.f);
}
