#include "PixelTriplet.hpp"
#include "triangle.hpp"

void zDepthCalc::setup(const float* sX, const float* sY, const float* zDepth) {
	compute(sX[0], sY[0], zDepth[0], sX[1], sY[1], zDepth[1], sX[2], sY[2], zDepth[2]);
}
#include <iostream>
void zDepthCalc::setup(const pixelTriplet* pixels) {
	setup(pixels->sX, pixels->sY, pixels->zDepth);
	/*std::cout << "x0,y0 : " << getZ(pixels->sX[0], pixels->sY[0]) << " (" << pixels->zDepth[0] << ")" << std::endl;
	std::cout << "x1,y1 : " << getZ(pixels->sX[1], pixels->sY[1]) << " (" << pixels->zDepth[1] << ")" << std::endl;
	std::cout << "x2,y2 : " << getZ(pixels->sX[2], pixels->sY[2]) << " (" << pixels->zDepth[2] << ")" << std::endl;*/
}

float zDepthCalc::getZ(const float& x, const float& y) const {
	return 1 / (A * x + B * y + C);
}

void zDepthCalc::compute(const float& x0, const float& y0, const float& z0,
	const float& x1, const float& y1, const float& z1,
	const float& x2, const float& y2, const float& z2) {
	/*float alpha = x2 - x0 + (y0 - y2) * (x1 - x0) / (y1 - y0);
	A = ((1 / z2) - (1 / z0) - y0 * (1 / z1 + 1 / z0) / (y1 - y0) - y2 * (1 / z1 - 1 / z0) / (y1 - y0)) / alpha;*/
	float alpha = (x2 - x0) - (x1 - x0) * (y2 - y0) / (y1 - y0);
	A = ((1 / z2) - (1 / z0) + ((1 / z0) - (1 / z1)) * (y2 - y0) / (y1 - y0)) / alpha;
	B = ((1 / z1) - (1 / z0) - A * (x1 - x0)) / (y1 - y0);
	C = (1 / z0) - A * x0 - B * y0;
}

vector3 pixelTriplet::getVertex0() const {
	if(offset)
		return (*tri->p0 + *offset);
	return *tri->p0;
}

vector3 pixelTriplet::getVertex1() const {
	if (offset)
		return (*tri->p1 + *offset);
	return *tri->p1;
}

vector3 pixelTriplet::getVertex2() const {
	if (offset)
		return (*tri->p2 + *offset);
	return *tri->p2;
}

vector3 pixelTriplet::getCenterPoint() const {
	if (offset)
		return (tri->p + *offset);
	return tri->p;
}

void pixelTriplet::set(const size_t& index, const int& x, const int& y) {
	pX[index] = x;
	pY[index] = y;
}

bool pixelTriplet::sortVertical(const int& yMax) {
	// Sort points by ascending Y
	if (pY[1] < pY[0]) {
		std::swap(pY[0], pY[1]);
		std::swap(pX[0], pX[1]);
	}
	if (pY[2] < pY[1]) {
		std::swap(pY[1], pY[2]);
		std::swap(pX[1], pX[2]);
		if (pY[1] < pY[0]) {
			std::swap(pY[1], pY[0]);
			std::swap(pX[1], pX[0]);
		}
	}

	// Check if the triangle is on the screen
	if (pY[2] < 0 || pY[0] >= yMax) // Entire triangle is off the top or bottom of the screen
		return false;

	return true;
}

bool pixelTriplet::getHorizontalLimits(const int& scanline, int& xA, int& xB) const {
	if (scanline < pY[0] || scanline > pY[2]) // Scanline does not pass through triangle
		return false;

	if (pY[0] == pY[1]) { // y10 is a horizontal line
		xA = (scanline - pY[1]) * ((pX[2] - pX[1])) / (pY[2] - pY[1]) + pX[1];
		xB = (scanline - pY[2]) * ((pX[0] - pX[2])) / (pY[0] - pY[2]) + pX[2];
	}
	else if (pY[0] == pY[2]) { // y20 is a horizontal line
		xA = (scanline - pY[1]) * ((pX[2] - pX[1])) / (pY[2] - pY[1]) + pX[1];
		xB = (scanline - pY[0]) * ((pX[1] - pX[0])) / (pY[1] - pY[0]) + pX[0];
	}
	else { // No lines are horizontal
		if (scanline <= pY[1])
			xA = (scanline - pY[0]) * ((pX[1] - pX[0])) / (pY[1] - pY[0]) + pX[0];
		else
			xA = (scanline - pY[1]) * ((pX[2] - pX[1])) / (pY[2] - pY[1]) + pX[1];
		xB = (scanline - pY[2]) * ((pX[0] - pX[2])) / (pY[0] - pY[2]) + pX[2];
	}

	if (xB < xA) // Sort xA and xB
		std::swap(xA, xB);

	// Check if the line is on the screen
	// TODO fix hard-coded screen limits
	if (xB < 0 || xA >= 640) { // Entire line is off the left or right of the screen
		return false;
	}

	if (xA < 0)
		xA = 0;
	if (xB >= 640)
		xB = 639;

	return true;
}

void pixelTriplet::finalize() {
	calc.setup(this);
}

bool  pixelTriplet::intersects(const ray& ray, float& t) const {
	float denom = ray.dir * tri->norm;
	if (denom == 0) // Normal is orthogonal to ray
		return false;
	float numer = (getCenterPoint() - ray.pos) * tri->norm;
	t = numer / denom;
	return (numer < 0 && denom < 0);
}