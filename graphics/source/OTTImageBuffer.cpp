#include <iostream> // TEMP
#include <algorithm>
#include <cmath>

#include "OTTImageBuffer.hpp"
#include "OTTLogicalColor.hpp"

constexpr float PI = 3.14159f;

OTTImageBuffer::~OTTImageBuffer() {
}

bool OTTImageBuffer::getPixel(const int& x, const int& y, OTTLogicalColor& color) {
	if (x >= nWidth || y >= nHeight)
		return false;
	color.pArray = &dptr[(nWidth * y + x) * nChannels];
	return true;
}

const unsigned char* OTTImageBuffer::getPixel(const int& x, const int& y) const {
	if (x >= nWidth || y >= nHeight)
		return 0x0;
	return &dptr[(nWidth * y + x) * nChannels];
}

void OTTImageBuffer::copyImageData(std::vector<unsigned char>& output) const {
	output = std::vector<unsigned char>(bitmap.cbegin(), bitmap.cend());
}

void OTTImageBuffer::resize(const unsigned short& W, const unsigned short& H, const unsigned short& ch){
	nWidth = W;
	nHeight = H;
	nChannels = ch;
	nSize = W * H;	
	if (nSize * nChannels != nBytes) {
		nBytes = nSize * nChannels;
		bitmap.resize(nBytes); // Contract size or expand, filling new pixels with zero
	}
	dptr = &bitmap[0]; // Image data may have been moved
}

void OTTImageBuffer::setPixel(const unsigned short& x, const unsigned short& y, const ColorRGB& color){
	unsigned char* base = get(x, y);
	for(unsigned short i = 0; i < nChannels; i++)
		base[i] = color[i];
}

void OTTImageBuffer::setPixel(const unsigned short& x, const unsigned short& y, const OTTLogicalColor& color) {
	unsigned char* base = get(x, y);
	for (unsigned short i = 0; i < nChannels; i++)
		base[i] = color[i];
}

void OTTImageBuffer::setPixelRow(const unsigned short& y, const ColorRGB& color){
	for(unsigned short i = 0; i < nWidth; i++)
		setPixel(i, y, color);
}

void OTTImageBuffer::setOpacity(const float& opacity) {
	if (opacity < 0.f || opacity > 1.f) // Opacity out of range
		return;
	OTTLogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.setAlpha(pixel.getA() * opacity);
		}
	}
}

void OTTImageBuffer::invertColors() {
	OTTLogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.invert();
		}
	}
}

void OTTImageBuffer::toGrayscale() {
	OTTLogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.toGrayscale();
		}
	}
}

void OTTImageBuffer::removeAlpha() {
	OTTLogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			pixel.setAlpha(1.f);
		}
	}
}

void OTTImageBuffer::colorToAlpha(const ColorRGB& chroma, const float& margin/*=0*/, const float& opacity/*=0*/) {
	OTTLogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			if (pixel.compareColor(chroma, margin))
				pixel.setAlpha(opacity);
		}
	}
}

void OTTImageBuffer::fillColor(const ColorRGB& color) {
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			blendPixel(j, i);
		}
	}
}

void OTTImageBuffer::drawSubImage(const unsigned short& x, const unsigned short& y, OTTImageBuffer* buffer) {
	OTTLogicalColor color;
	for (unsigned short i = 0; i < buffer->getHeight(); i++) { // Over rows
		if (y + i >= nHeight) // Invalid image coordinates
			break;
		for (unsigned short j = 0; j < buffer->getWidth(); j++) { // Over columns
			if (!buffer->getPixel(j, i, color))
				break; // Invalid sub-image coordinates
			setPixel(x + j, y + i, color);
		}
	}
}

void OTTImageBuffer::drawPixel(const unsigned short& x, const unsigned short& y) {
	blendPixel(x, y);
}

void OTTImageBuffer::drawPixels(const std::vector<std::pair<unsigned short, unsigned short> >& vertices) {
	for (auto vert = vertices.cbegin(); vert != vertices.cend(); vert++) {
		blendPixel(vert->first, vert->second);
	}
}

void OTTImageBuffer::drawPixels(const unsigned short* vertices, const size_t& N) {
	for (size_t i = 0; i < N; i++) {
		blendPixel(vertices[2 * i], vertices[2 * i + 1]);
	}
}

void OTTImageBuffer::drawPixels(const unsigned short& x, const unsigned short& y, OTTImageBuffer* buffer){
	OTTLogicalColor color;
	for (unsigned short i = 0; i < nHeight; i++) { // Over rows
		if (y + i >= nHeight) // Invalid image coordinates
			break;
		for (unsigned short j = 0; j < nWidth; j++) { // Over columns
			if (!buffer->getPixel(x + j, y + i, color))
				break; // Invalid sub-image coordinates
			setPixel(x, y, color);
		}
	}
}

void OTTImageBuffer::drawLine(
	const unsigned short& x0, const unsigned short& y0,
	const unsigned short& x1, const unsigned short& y1
) {
	if (y1 == y0) { // Horizontal line		
		if (x0 < x1) { // Sort X coordinates
			for (unsigned short dx = x0; dx <= x1; dx++)
				blendPixel(dx, y0);
		}
		else {
			for (unsigned short dx = x1; dx <= x0; dx++)
				blendPixel(dx, y0);
		}
		return;
	}

	if (x1 == x0) { // Vertical line
		if (y0 < y1) { // Sort Y coordinates
			for (unsigned short dy = y0; dy <= y1; dy++)
				blendPixel(x0, dy);
		}
		else {
			for (unsigned short dy = y1; dy <= y0; dy++)
				blendPixel(x0, dy);
		}
		return;
	}

	// Rasterize the input line. Input line is now guaranteed to not be vertical or horizontal
	const float fSlope = ((float)y1 - (float)y0) / ((float)x1 - (float)x0); // dY per dX step
	if (std::abs(fSlope) > 1.f) { // dY > dX
		if (y0 < y1) {
			float fX = (float)x0;
			for (unsigned short i = y0; i <= y1; i++) { // Stepping down from (x0,y0) to (x1,y1)
				blendPixel((unsigned short)fX, i);
				fX += (1.f / fSlope);
			}
		}
		else {
			float fX = (float)x1;
			for (unsigned short i = y1; i <= y0; i++) { // Stepping down from (x1,y1) to (x0,y0)
				blendPixel((unsigned short)fX, i);
				fX += (1.f / fSlope);
			}
		}
	}
	else { // dX > dY
		if (x0 < x1) {
			float fY = (float)y0;
			for (unsigned short i = x0; i <= x1; i++) { // Stepping right from (x0,y0) to (x1,y1)
				blendPixel(i, (unsigned short)fY);
				fY += fSlope;
			}
		}
		else {
			float fY = (float)y1;
			for (unsigned short i = x1; i <= x0; i++) { // Stepping right from (x1,y1) to (x0,y0)
				blendPixel(i, (unsigned short)fY);
				fY += fSlope;
			}
		}
	}
}

void OTTImageBuffer::drawLines(const std::vector<std::pair<unsigned short, unsigned short> >& vertices) {
	if (vertices.size() < 2)
		return;
	unsigned short prevX = vertices[0].first;
	unsigned short prevY = vertices[0].second;
	for (auto vert = vertices.cbegin() + 1; vert != vertices.cend(); vert++) {
		drawLine(prevX, prevY, vert->first, vert->second);
		prevX = vert->first;
		prevY = vert->second;
	}
}

void OTTImageBuffer::drawLines(const unsigned short* vertices, const size_t& N) {
	if (N < 2)
		return;
	unsigned short prevX = vertices[0];
	unsigned short prevY = vertices[1];
	for (size_t i = 1; i < N; i++){
		drawLine(prevX, prevY, vertices[2 * i], vertices[2 * i + 1]);
		prevX = vertices[2 * i];
		prevY = vertices[2 * i + 1];
	}
}

void OTTImageBuffer::drawPolygon(const std::vector<std::pair<unsigned short, unsigned short> >& vertices) {
	if (vertices.size() < 3)
		return;
	drawLines(vertices); // Draw the edges
	drawLine(
		vertices.back().first, vertices.back().second, 
		vertices.front().first, vertices.front().second
	); // Close the polygon
}

void OTTImageBuffer::drawPolygon(const unsigned short* vertices, const size_t& N) {
	if (N < 3)
		return;
	drawLines(vertices, N); // Draw the edges
	drawLine(
		vertices[2 * (N - 1)], vertices[2 * (N - 1) + 1],
		vertices[0], vertices[1]
	); // Close the polygon
}

void OTTImageBuffer::drawRectangle(
	const unsigned short& x0, const unsigned short& y0,
	const unsigned short& x1, const unsigned short& y1,
	bool bFilled/* = false*/
) {
	if (!bFilled) {
		unsigned short vertices[8] = {
			x0, y0, // Top left
			x1, y0, // Top right
			x1, y1, // Bottom right
			x0, y1  // Bottom left
		};
		drawPolygon(vertices, 4);
	}
	else {
		for (unsigned short y = y0; y <= y1; y++) {
			drawLine(x0, y, x1, y);
		}
	}
}

void OTTImageBuffer::drawCircle(const float& x0, const float& y0, const float& radius, bool bFilled/* = false*/) {
	if (radius <= 0.f)
		return;
	// Cardinal directions
	// N(0): x0, y0 + r
	// E(1): x0 + r, y0
	// S(2): x0, y0 - r
	// W(3): x0 - r, y0
	float xStart[4] = { x0, x0 + radius, x0, x0 - radius };
	float yStart[4] = { y0 + radius, y0, y0 - radius, y0 };
	unsigned short xStop = (unsigned short)(radius * 0.70710678f); // sqrt(2) / 2
	float fd1, fd2;
	for (unsigned short x = 0; x <= xStop; x++) {
		if (!bFilled) {
			for (int i = 0; i < 4; i++) {
				blendPixel(xStart[i], yStart[i]); // Primary pixels
				blendPixel(2.f * x0 - xStart[i], yStart[i]); // Mirror pixels
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				drawLine( xStart[i], yStart[i], 2 * x0 - xStart[i], yStart[i]);
			}
		}
		// Step "right" or step "down"?
		float fdyy1 = yStart[0] - y0; 
		float fdyy2 = yStart[0] - 1 - y0; 
		float fdxx = xStart[0] + 1 - x0; 
		fdxx *= fdxx;
		fd1 = std::fabs(fdxx + fdyy1 * fdyy1 - radius * radius);
		fd2 = std::fabs(fdxx + fdyy2 * fdyy2 - radius * radius);
		if (fd2 < fd1) { // Update all octants (step "down")
			xStart[0] += 1; yStart[0] -= 1; // 0: x+1, y-1 (SE)
			xStart[1] -= 1; yStart[1] -= 1; // 2: x-1, y-1 (SW)
			xStart[2] -= 1; yStart[2] += 1; // 4: x-1, y+1 (NW)
			xStart[3] += 1; yStart[3] += 1; // 6: x+1, y+1 (NE)
		}
		else { // Update all octants (step "right")
			xStart[0] += 1; // 0: x+1, y (E)
			yStart[1] -= 1; // 2: x, y+1 (S)
			xStart[2] -= 1; // 4: x-1, y (W)
			yStart[3] += 1; // 6: x, y+1 (N)
		}
	}
}

void OTTImageBuffer::drawEllipse(const unsigned short& x0, const unsigned short& y0, const float& A, const float& B, bool bFilled/* = false*/) {
	if (A <= 0.f || B <= 0.f)
		return;
	float xStart[2] = { x0, x0 };
	float yStart[2] = { y0 + B, y0 - B };
	float fd1, fd2, fd3;
	unsigned short xStop = (unsigned short)(x0 + A);
	unsigned short maxSteps = (unsigned short)(A + B); // Absolute maximum number of steps (square)
	for(unsigned short step = 0; step <= maxSteps; step++){ // Prevent infinite loops
		if (!bFilled) {
			for (int i = 0; i < 2; i++) {
				blendPixel(xStart[i], yStart[i]); // Primary pixels
				blendPixel(2.f * x0 - xStart[i], yStart[i]); // Mirror pixels
			}
		}
		else {
			for (int i = 0; i < 2; i++) {
				drawLine(xStart[i], yStart[i], 2 * x0 - xStart[i], yStart[i]);
			}
		}
		if (xStart[0] >= xStop && yStart[0] <= y0) // Goal reached
			break;
		// There are three possible directions which we could step
		float fdyy1 = yStart[0] - y0; // East
		float fdyy2 = yStart[0] - 1 - y0; // South / Southeast
		float fdxx1 = xStart[0] + 1 - x0; // East / Southeast
		float fdxx2 = xStart[0] - x0; // South
		fdxx1 *= fdxx1 / (A * A);
		fdxx2 *= fdxx2 / (A * A);
		fdyy1 *= fdyy1 / (B * B);
		fdyy2 *= fdyy2 / (B * B);
		fd1 = std::fabs(fdxx1 + fdyy1 - 1.f); // East
		fd2 = std::fabs(fdxx1 + fdyy2 - 1.f); // Southeast
		fd3 = std::fabs(fdxx2 + fdyy2 - 1.f); // South
		if (fd1 < fd2) { // East or South
			if (fd1 < fd3) { // East
				xStart[0] += 1; // 0: x+1, y (E)
				xStart[1] -= 1; // 4: x-1, y (W)
			}
			else { // South
				yStart[0] -= 1; // 0: x+1, y (E)
				yStart[1] += 1; // 4: x-1, y (W)
			}
		}
		else { // Southeast or South
			if (fd2 < fd3) { // Southeast
				xStart[0] += 1; yStart[0] -= 1; // 0: x+1, y-1 (SE)
				xStart[1] -= 1; yStart[1] += 1; // 4: x-1, y+1 (NW)
			}
			else { // South
				yStart[0] -= 1; // 0: x+1, y (E)
				yStart[1] += 1; // 4: x-1, y (W)
			}
		}
	}
}

void OTTImageBuffer::drawRegularPolygon(const unsigned short& x0, const unsigned short& y0,	const unsigned short& N, const float& radius) {
	if (N < 3)
		return;
	const float dTheta = 2.f * PI / N;
	std::vector<unsigned short> vertices;
	float px = 0.f;
	float py = 0.f;
	float theta = 0.f;
	for (unsigned short i = 0; i < N; i++) {
		vertices.push_back((unsigned short)(radius * std::sin(theta) + x0));
		vertices.push_back((unsigned short)(radius * std::cos(theta) + y0));
		theta += dTheta;
	}
	drawPolygon(vertices.data(), N);
}

void OTTImageBuffer::drawTriangle(
	const unsigned short& x0, const unsigned short& y0,
	const unsigned short& x1, const unsigned short& y1,
	const unsigned short& x2, const unsigned short& y2
) {
	drawLine(x0, y0, x1, y1);
	drawLine(x1, y1, x2, y2);
	drawLine(x2, y2, x0, y0);
}

void OTTImageBuffer::drawQuad(
	const unsigned short& x0, const unsigned short& y0,
	const unsigned short& x1, const unsigned short& y1,
	const unsigned short& x2, const unsigned short& y2,
	const unsigned short& x3, const unsigned short& y3
) {
	drawLine(x0, y0, x1, y1);
	drawLine(x1, y1, x2, y2);
	drawLine(x2, y2, x3, y3);
	drawLine(x3, y3, x0, y0);
}

void OTTImageBuffer::processImage(imageManipulationFunc func) {
	OTTLogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			getPixel(j, i, pixel);
			(*func)(&pixel, j, i);
		}
	}
}

void OTTImageBuffer::clear(const ColorRGB& color){
	for(unsigned int i = 0; i < nSize; i++){
		for(unsigned short j = 0; j < nChannels; j++)
			dptr[nChannels * i + j] = color[j];
	}
}

void OTTImageBuffer::fill(const unsigned char& value/*=0*/){
	std::fill(bitmap.begin(), bitmap.end(), value);
}

void OTTImageBuffer::free(){
	bitmap.resize(0);
	dptr = 0x0;
}

size_t OTTImageBuffer::getImageTargets(
	const int& x, const int& y,
	OTTImageBuffer* dest,
	OTTImageBuffer* src,
	std::vector<std::pair<unsigned char*, unsigned char*> >& targets)
{
	return getImageTargets(x, y, 0, 0, src->getWidth(), src->getHeight(), dest, src, targets);
}

size_t OTTImageBuffer::getImageTargets(
	const int& x0, const int& y0,
	const int& x1, const int& y1,
	const int& W, const int& H,
	OTTImageBuffer* dest,
	OTTImageBuffer* src,
	std::vector<std::pair<unsigned char*, unsigned char*> >& targets)
{
	if (
		(dest->getNumChannels() != src->getNumChannels())
		|| ((x0 + W) > dest->getWidth() || (x1 + W) > src->getWidth())
		|| ((y0 + H) > dest->getHeight() || (y1 + H) > src->getHeight())
	)
		return 0;
	targets.clear();
	targets.reserve(H);
	for (int i = 0; i < H; i++) {
		unsigned char* destPixel = dest->get(x0, y0 + i);
		unsigned char* srcPixel = src->get(x1, y1 + i);
		if (!destPixel || !srcPixel) // Invalid location
			return targets.size();
		targets.push_back(std::make_pair(destPixel, srcPixel));
	}
	return targets.size();
}

void OTTImageBuffer::blendPixel(const unsigned short& px, const unsigned short& py) {
	OTTLogicalColor pixel(nChannels >= 4);
	if (!getPixel(px, py, pixel)) // Invalid pixel position
		return;
	switch (colorBlendMode) {
	case BlendMode::NONE:
		break;
	case BlendMode::NORMAL:
		pixel.setColor(currentDrawColor);
		break;
	case BlendMode::ADD:
		pixel += currentDrawColor;
		break;
	case BlendMode::SUB:
		pixel -= currentDrawColor;
		break;
	case BlendMode::MULT:
		pixel *= currentDrawColor;
		break;
	case BlendMode::DIVIDE:
		pixel /= currentDrawColor;
		break;
	case BlendMode::DIFF:
		pixel.difference(currentDrawColor);
		break;
	case BlendMode::DARKEN:
		pixel.darken(currentDrawColor);
		break;
	case BlendMode::LIGHTEN:
		pixel.lighten(currentDrawColor);
		break;
	case BlendMode::AVERAGE:
		pixel.average(currentDrawColor);
		break;
	case BlendMode::REPLACE:
		pixel.setColor(currentDrawColor);
		break;
	default: // Unknown blending mode. Do nothing
		break;
	}
}
