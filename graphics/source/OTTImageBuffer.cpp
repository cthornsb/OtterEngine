#include <algorithm>
#include <cmath>

#include "OTTImageBuffer.hpp"
#include "OTTLogicalColor.hpp"

constexpr float PI = 3.14159f;

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

void OTTImageBuffer::resize(const unsigned short& W, const unsigned short& H){
	nWidth = W;
	nHeight = H;
	nSize = W * H;
	nBytes = nSize * nChannels;
	bitmap.resize(nBytes); // Contract size or expand, filling new pixels with zero
	dptr = &bitmap[0]; // Image data may have been moved
}

void OTTImageBuffer::setPixel(const unsigned short& x, const unsigned short& y, const ColorRGB& color){
	unsigned char* base = get(x, y);
	for(unsigned short i = 0; i < nChannels; i++)
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

	if (x1 == 0) { // Vertical line
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
	const unsigned short& x1, const unsigned short& y1
) {
	unsigned short vertices[8] = {
		x0, y0, // Top left
		x1, y0, // Top right
		x1, y1, // Bottom right
		x0, y1  // Bottom left
	};
	drawPolygon(vertices, 4);
}

void OTTImageBuffer::drawCircle(const unsigned short& x0, const unsigned short& y0, const float& radius) {
	if (radius < 0.f)
		return;
	float fX[2] = { (float)x0 - radius, (float)x0 + radius };
	float fY[2] = { (float)y0 - radius, (float)y0 + radius };
	unsigned short xBounds[2] = { 
		(fX[0] >= 0.f ? (unsigned short)fX[0] : (unsigned short)0),
		(fX[1] < (float)nWidth ? (unsigned short)fX[1] : nWidth - 1)
	};
	unsigned short yBounds[2] = {
		(fY[0] >= 0.f ? (unsigned short)fY[0] : (unsigned short)0),
		(fY[1] < (float)nHeight ? (unsigned short)fY[1] : nHeight - 1)
	};
	float fCenterX = (float)x0;
	float fCenterY = (float)y0;
	float fPX = (float)fX[0];
	float fPY = (float)fY[0];
	for (unsigned short y = yBounds[0]; y <= yBounds[1]; y++) { // Over all rows
		fPX = fX[0];
		for (unsigned short x = xBounds[0]; x <= xBounds[1]; x++) { // Over all columns
			if (std::sqrt((fPX - fCenterX) * (fPX - fCenterX) + (fPY - fCenterY) * (fPY - fCenterY)) <= radius) // Fill in the circle
				blendPixel((unsigned short)fPX, (unsigned short)fPY);
			fPX += 1.f;
		}
		fPY += 1.f;
	}
}

void OTTImageBuffer::drawEllipse(const unsigned short& x0, const unsigned short& y0, const float& A, const float& B) {
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
		vertices.push_back((unsigned short)(radius * std::sin(theta)));
		vertices.push_back((unsigned short)(radius * std::cos(theta)));
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

void OTTImageBuffer::blendPixel(const unsigned short& px, const unsigned short& py) {
	OTTLogicalColor pixel(nChannels >= 4);
	if (!getPixel(px, nHeight - py, pixel)) // Invalid pixel position
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