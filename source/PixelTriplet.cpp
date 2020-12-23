#include <algorithm>

#include "PixelTriplet.hpp"
#include "lightSource.hpp"
#include "triangle.hpp"

void zDepthCalc::setup(const float* sX, const float* sY, const float* zDepth) {
	compute(sX[0], sY[0], zDepth[0], sX[1], sY[1], zDepth[1], sX[2], sY[2], zDepth[2]);
}

void zDepthCalc::setup(const pixelTriplet& pixels) {
	compute(pixels[0]->sX, pixels[0]->sY, pixels[0]->zDepth,
		pixels[1]->sX, pixels[1]->sY, pixels[1]->zDepth,
		pixels[2]->sX, pixels[2]->sY, pixels[2]->zDepth);
}

float zDepthCalc::getZ(const float& x, const float& y) const {
	return 1 / (A * x + B * y + C);
}

void zDepthCalc::compute(const float& x0, const float& y0, const float& z0,
	const float& x1, const float& y1, const float& z1,
	const float& x2, const float& y2, const float& z2) 
{
	float alpha = (x2 - x0) - (x1 - x0) * (y2 - y0) / (y1 - y0);
	A = ((1 / z2) - (1 / z0) + ((1 / z0) - (1 / z1)) * (y2 - y0) / (y1 - y0)) / alpha;
	B = ((1 / z1) - (1 / z0) - A * (x1 - x0)) / (y1 - y0);
	C = (1 / z0) - A * x0 - B * y0;
}

pixelTriplet::pixelTriplet() :
	tri(0x0),
	p0(0x0),
	p1(0x0),
	p2(0x0),
	calc(),
	draw()
{
}

/** Constructor taking a pointer to a 3d triangle
  */
pixelTriplet::pixelTriplet(triangle* t) :
	tri(t),
	p0(t->p0),
	p1(t->p1),
	p2(t->p2),
	calc(),
	draw()
{
}

Vertex* pixelTriplet::operator [] (const size_t& index) const {
	switch (index) {
	case 0:
		return p0;
	case 1:
		return p1;
	case 2:
		return p2;
	default:
		break;
	}
	return 0x0;
}

vector3 pixelTriplet::getCenterPoint() const {
	return tri->getCenterPoint();
}

void pixelTriplet::set(const size_t& index, const int& x, const int& y) {
	(*this)[index]->pX = x;
	(*this)[index]->pY = y;
}

bool pixelTriplet::sortVertical(const int& yMax) {
	// Sort points by ascending Y
	if (p1->pY < p0->pY) {
		std::swap(p0, p1);
	}
	if (p2->pY < p1->pY) {
		std::swap(p1, p2);
		if (p1->pY < p0->pY) {
			std::swap(p1, p0);
		}
	}

	// Triangle is a line?
	if (p0->pY == p2->pY)
		return false;

	// Check if the triangle is on the screen
	if (p2->pY < 0 || p0->pY >= yMax) // Entire triangle is off the top or bottom of the screen
		return false;

	return true;
}

bool pixelTriplet::getHorizontalLimits(const int& scanline, int& xA, int& xB) const {
	if (scanline < p0->pY || scanline > p2->pY) // Scanline does not pass through triangle
		return false;

	if (p0->pY == p1->pY) { // y10 is a horizontal line
		xA = (scanline - p1->pY) * ((p2->pX - p1->pX)) / (p2->pY - p1->pY) + p1->pX;
		xB = (scanline - p2->pY) * ((p0->pX - p2->pX)) / (p0->pY - p2->pY) + p2->pX;
	}
	else if (p0->pY == p2->pY) { // y20 is a horizontal line
		xA = (scanline - p1->pY) * ((p2->pX - p1->pX)) / (p2->pY - p1->pY) + p1->pX;
		xB = (scanline - p0->pY) * ((p1->pX - p0->pX)) / (p1->pY - p0->pY) + p0->pX;
	}
	else { // No lines are horizontal
		if (scanline <= p1->pY)
			xA = (scanline - p0->pY) * ((p1->pX - p0->pX)) / (p1->pY - p0->pY) + p0->pX;
		else
			xA = (scanline - p1->pY) * ((p2->pX - p1->pX)) / (p2->pY - p1->pY) + p1->pX;
		xB = (scanline - p2->pY) * ((p0->pX - p2->pX)) / (p0->pY - p2->pY) + p2->pX;
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

void pixelTriplet::computeLighting(lightSource* light) {
	p0->light += light->getColor(*this);
	p1->light += light->getColor(*this);
	p2->light += light->getColor(*this);
}

void pixelTriplet::resetLighting() {
	p0->light.reset();
	p1->light.reset();
	p2->light.reset();
}

void pixelTriplet::finalize() {
	calc.setup(*this);
}

bool  pixelTriplet::intersects(const ray& ray, float& t) const {
	float denom = ray.dir * tri->norm;
	if (denom == 0) // Normal is orthogonal to ray
		return false;
	float numer = (getCenterPoint() - ray.pos) * tri->norm;
	t = numer / denom;
	return (numer < 0 && denom < 0);
}