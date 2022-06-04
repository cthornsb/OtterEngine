#include "OTTImageBuffer.hpp"
#include "OTTLogicalColor.hpp"

#include <algorithm>
#include <cmath>

constexpr float PI = 3.14159f;

ott::ImageBuffer::~ImageBuffer() {
}

bool ott::ImageBuffer::GetPixel(const int& x, const int& y, LogicalColor& color) {
	if (x >= nWidth || y >= nHeight)
		return false;
	color.pArray = &dptr[(nWidth * y + x) * nChannels];
	return true;
}

const unsigned char* ott::ImageBuffer::GetPixel(const int& x, const int& y) const {
	if (x >= nWidth || y >= nHeight)
		return 0x0;
	return &dptr[(nWidth * y + x) * nChannels];
}

void ott::ImageBuffer::CopyImageData(std::vector<unsigned char>& output) const {
	output = std::vector<unsigned char>(bitmap.cbegin(), bitmap.cend());
}

void ott::ImageBuffer::Resize(const unsigned short& W, const unsigned short& H, const unsigned short& ch){
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

void ott::ImageBuffer::SetPixel(const unsigned short& x, const unsigned short& y, const ColorRGB& color){
	uint8_t* base = this->Get(x, y);
	for (uint8_t i = 0; i < nChannels; i++) {
		base[i] = color[i];
	}
}

void ott::ImageBuffer::SetPixel(const unsigned short& x, const unsigned short& y, const LogicalColor& color) {
	uint8_t* base = this->Get(x, y);
	for (uint8_t i = 0; i < nChannels; i++) {
		base[i] = color[i];
	}
}

void ott::ImageBuffer::SetPixelRow(const unsigned short& y, const ColorRGB& color){
	for(unsigned short i = 0; i < nWidth; i++)
		this->SetPixel(i, y, color);
}

void ott::ImageBuffer::SetOpacity(const float& opacity) {
	if (opacity < 0.f || opacity > 1.f) // Opacity out of range
		return;
	LogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			this->GetPixel(j, i, pixel);
			pixel.SetA(pixel.A() * opacity);
		}
	}
}

void ott::ImageBuffer::InvertColors() {
	LogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			this->GetPixel(j, i, pixel);
			pixel.Invert();
		}
	}
}

void ott::ImageBuffer::ToGrayscale() {
	LogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			this->GetPixel(j, i, pixel);
			pixel.ToGrayscale();
		}
	}
}

void ott::ImageBuffer::RemoveAlpha() {
	LogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			this->GetPixel(j, i, pixel);
			pixel.SetA(1.f);
		}
	}
}

void ott::ImageBuffer::ColorToAlpha(const ColorRGB& chroma, const float& margin/*=0*/, const float& opacity/*=0*/) {
	LogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			this->GetPixel(j, i, pixel);
			if (pixel.CompareColor(chroma, margin))
				pixel.SetA(opacity);
		}
	}
}

void ott::ImageBuffer::FillColor(const ColorRGB& color) {
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			this->BlendPixel(j, i);
		}
	}
}

void ott::ImageBuffer::BlendSubImage(const unsigned short& x, const unsigned short& y, ImageBuffer* buffer, const float& alpha) {
	LogicalColor dest(nChannels >= 4);
	LogicalColor src(buffer->NumChannels() >= 4);
	for (unsigned short i = 0; i < buffer->Height(); i++) { // Over rows
		if (y + i >= nHeight) // Invalid image coordinates
			break;
		for (unsigned short j = 0; j < buffer->Width(); j++) { // Over columns
			if (!this->GetPixel(x + j, y + i, dest) || !buffer->GetPixel(j, i, src)) // Invalid pixel position
				return;
			src.SetA(alpha);
			this->BlendPixel(dest, src);
		}
	}
}

void ott::ImageBuffer::BlendSubImage(const unsigned short& x, const unsigned short& y, ImageBuffer* buffer) {
	LogicalColor dest(nChannels >= 4);
	LogicalColor src(buffer->NumChannels() >= 4);
	for (unsigned short i = 0; i < buffer->Height(); i++) { // Over rows
		if (y + i >= nHeight) // Invalid image coordinates
			break;
		for (unsigned short j = 0; j < buffer->Width(); j++) { // Over columns
			if (!this->GetPixel(x + j, y + i, dest) || !buffer->GetPixel(j, i, src)) // Invalid pixel position
				return;
			this->BlendPixel(dest, src);
		}
	}
}

void ott::ImageBuffer::DrawSubImage(const unsigned short& x, const unsigned short& y, ImageBuffer* buffer) {
	LogicalColor dest(nChannels >= 4);
	LogicalColor src(buffer->NumChannels() >= 4);
	for (unsigned short i = 0; i < buffer->Height(); i++) { // Over rows
		if (y + i >= nHeight) // Invalid image coordinates
			break;
		for (unsigned short j = 0; j < buffer->Width(); j++) { // Over columns
			if (!this->GetPixel(x + j, y + i, dest) || !buffer->GetPixel(j, i, src)) // Invalid pixel position
				return;
			dest.Set(src);
		}
	}
}

void ott::ImageBuffer::DrawPixel(const unsigned short& x, const unsigned short& y) {
	this->BlendPixel(x, y);
}

void ott::ImageBuffer::DrawPixels(const std::vector<std::pair<unsigned short, unsigned short> >& vertices) {
	for (auto vert = vertices.cbegin(); vert != vertices.cend(); vert++) {
		this->BlendPixel(vert->first, vert->second);
	}
}

void ott::ImageBuffer::DrawPixels(const unsigned short* vertices, const size_t& N) {
	for (size_t i = 0; i < N; i++) {
		this->BlendPixel(vertices[2 * i], vertices[2 * i + 1]);
	}
}

void ott::ImageBuffer::DrawPixels(const unsigned short& x, const unsigned short& y, ImageBuffer* buffer){
	LogicalColor color;
	for (unsigned short i = 0; i < nHeight; i++) { // Over rows
		if (y + i >= nHeight) // Invalid image coordinates
			break;
		for (unsigned short j = 0; j < nWidth; j++) { // Over columns
			if (!buffer->GetPixel(x + j, y + i, color))
				break; // Invalid sub-image coordinates
			this->SetPixel(x, y, color);
		}
	}
}

void ott::ImageBuffer::DrawLine(
	const unsigned short& x0, const unsigned short& y0,
	const unsigned short& x1, const unsigned short& y1
) {
	if (y1 == y0) { // Horizontal line		
		if (x0 < x1) { // Sort X coordinates
			for (unsigned short dx = x0; dx <= x1; dx++)
				this->BlendPixel(dx, y0);
		}
		else {
			for (unsigned short dx = x1; dx <= x0; dx++)
				this->BlendPixel(dx, y0);
		}
		return;
	}

	if (x1 == x0) { // Vertical line
		if (y0 < y1) { // Sort Y coordinates
			for (unsigned short dy = y0; dy <= y1; dy++)
				this->BlendPixel(x0, dy);
		}
		else {
			for (unsigned short dy = y1; dy <= y0; dy++)
				this->BlendPixel(x0, dy);
		}
		return;
	}

	// Rasterize the input line. Input line is now guaranteed to not be vertical or horizontal
	const float fSlope = ((float)y1 - (float)y0) / ((float)x1 - (float)x0); // dY per dX step
	if (std::abs(fSlope) > 1.f) { // dY > dX
		if (y0 < y1) {
			float fX = (float)x0;
			for (unsigned short i = y0; i <= y1; i++) { // Stepping down from (x0,y0) to (x1,y1)
				this->BlendPixel((unsigned short)fX, i);
				fX += (1.f / fSlope);
			}
		}
		else {
			float fX = (float)x1;
			for (unsigned short i = y1; i <= y0; i++) { // Stepping down from (x1,y1) to (x0,y0)
				this->BlendPixel((unsigned short)fX, i);
				fX += (1.f / fSlope);
			}
		}
	}
	else { // dX > dY
		if (x0 < x1) {
			float fY = (float)y0;
			for (unsigned short i = x0; i <= x1; i++) { // Stepping right from (x0,y0) to (x1,y1)
				this->BlendPixel(i, (unsigned short)fY);
				fY += fSlope;
			}
		}
		else {
			float fY = (float)y1;
			for (unsigned short i = x1; i <= x0; i++) { // Stepping right from (x1,y1) to (x0,y0)
				this->BlendPixel(i, (unsigned short)fY);
				fY += fSlope;
			}
		}
	}
}

void ott::ImageBuffer::DrawLines(const std::vector<std::pair<unsigned short, unsigned short> >& vertices) {
	if (vertices.size() < 2)
		return;
	unsigned short prevX = vertices[0].first;
	unsigned short prevY = vertices[0].second;
	for (auto vert = vertices.cbegin() + 1; vert != vertices.cend(); vert++) {
		this->DrawLine(prevX, prevY, vert->first, vert->second);
		prevX = vert->first;
		prevY = vert->second;
	}
}

void ott::ImageBuffer::DrawLines(const unsigned short* vertices, const size_t& N) {
	if (N < 2)
		return;
	unsigned short prevX = vertices[0];
	unsigned short prevY = vertices[1];
	for (size_t i = 1; i < N; i++){
		this->DrawLine(prevX, prevY, vertices[2 * i], vertices[2 * i + 1]);
		prevX = vertices[2 * i];
		prevY = vertices[2 * i + 1];
	}
}

void ott::ImageBuffer::DrawPolygon(const std::vector<std::pair<unsigned short, unsigned short> >& vertices) {
	if (vertices.size() < 3)
		return;
	this->DrawLines(vertices); // Draw the edges
	this->DrawLine(
		vertices.back().first, vertices.back().second, 
		vertices.front().first, vertices.front().second
	); // Close the polygon
}

void ott::ImageBuffer::DrawPolygon(const unsigned short* vertices, const size_t& N) {
	if (N < 3)
		return;
	this->DrawLines(vertices, N); // Draw the edges
	this->DrawLine(
		vertices[2 * (N - 1)], vertices[2 * (N - 1) + 1],
		vertices[0], vertices[1]
	); // Close the polygon
}

void ott::ImageBuffer::DrawRectangle(
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
		this->DrawPolygon(vertices, 4);
	}
	else {
		for (unsigned short y = y0; y <= y1; y++) {
			this->DrawLine(x0, y, x1, y);
		}
	}
}

void ott::ImageBuffer::DrawCircle(const float& x0, const float& y0, const float& radius, bool bFilled/* = false*/) {
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
				this->BlendPixel(xStart[i], yStart[i]); // Primary pixels
				this->BlendPixel(2.f * x0 - xStart[i], yStart[i]); // Mirror pixels
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				this->DrawLine( xStart[i], yStart[i], 2 * x0 - xStart[i], yStart[i]);
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

void ott::ImageBuffer::DrawEllipse(const unsigned short& x0, const unsigned short& y0, const float& A, const float& B, bool bFilled/* = false*/) {
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
				this->BlendPixel(xStart[i], yStart[i]); // Primary pixels
				this->BlendPixel(2.f * x0 - xStart[i], yStart[i]); // Mirror pixels
			}
		}
		else {
			for (int i = 0; i < 2; i++) {
				this->DrawLine(xStart[i], yStart[i], 2 * x0 - xStart[i], yStart[i]);
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

void ott::ImageBuffer::DrawRegularPolygon(const unsigned short& x0, const unsigned short& y0,	const unsigned short& N, const float& radius) {
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
	this->DrawPolygon(vertices.data(), N);
}

void ott::ImageBuffer::DrawTriangle(
	const unsigned short& x0, const unsigned short& y0,
	const unsigned short& x1, const unsigned short& y1,
	const unsigned short& x2, const unsigned short& y2
) {
	this->DrawLine(x0, y0, x1, y1);
	this->DrawLine(x1, y1, x2, y2);
	this->DrawLine(x2, y2, x0, y0);
}

void ott::ImageBuffer::DrawQuad(
	const unsigned short& x0, const unsigned short& y0,
	const unsigned short& x1, const unsigned short& y1,
	const unsigned short& x2, const unsigned short& y2,
	const unsigned short& x3, const unsigned short& y3
) {
	this->DrawLine(x0, y0, x1, y1);
	this->DrawLine(x1, y1, x2, y2);
	this->DrawLine(x2, y2, x3, y3);
	this->DrawLine(x3, y3, x0, y0);
}

void ott::ImageBuffer::ProcessImage(ImageManipulationFunc_t func) {
	LogicalColor pixel(nChannels >= 4);
	for (int i = 0; i < nHeight; i++) { // Over rows
		for (int j = 0; j < nWidth; j++) { // Over columns
			this->GetPixel(j, i, pixel);
			(*func)(&pixel, j, i);
		}
	}
}

void ott::ImageBuffer::Clear(const ColorRGB& color){
	for(unsigned int i = 0; i < nSize; i++){
		for(unsigned short j = 0; j < nChannels; j++)
			dptr[nChannels * i + j] = color[j];
	}
}

void ott::ImageBuffer::Fill(const unsigned char& value/*=0*/){
	std::fill(bitmap.begin(), bitmap.end(), value);
}

void ott::ImageBuffer::Free(){
	bitmap.resize(0);
	dptr = 0x0;
}

size_t ott::ImageBuffer::GetImageTargets(
	const int& x, const int& y,
	ImageBuffer* dest,
	ImageBuffer* src,
	std::vector<std::pair<unsigned char*, unsigned char*> >& targets)
{
	return ott::ImageBuffer::GetImageTargets(x, y, 0, 0, src->Width(), src->Height(), dest, src, targets);
}

size_t ott::ImageBuffer::GetImageTargets(
	const int& x0, const int& y0,
	const int& x1, const int& y1,
	const int& W, const int& H,
	ImageBuffer* dest,
	ImageBuffer* src,
	std::vector<std::pair<unsigned char*, unsigned char*> >& targets)
{
	if (
		(dest->NumChannels() != src->NumChannels())
		|| ((x0 + W) > dest->Width() || (x1 + W) > src->Width())
		|| ((y0 + H) > dest->Height() || (y1 + H) > src->Height())
	)
		return 0;
	targets.clear();
	targets.reserve(H);
	for (int i = 0; i < H; i++) {
		unsigned char* destPixel = dest->Get(x0, y0 + i);
		unsigned char* srcPixel = src->Get(x1, y1 + i);
		if (!destPixel || !srcPixel) // Invalid location
			return targets.size();
		targets.push_back(std::make_pair(destPixel, srcPixel));
	}
	return targets.size();
}

void ott::ImageBuffer::BlendPixel(const unsigned short& px, const unsigned short& py) {
	this->BlendPixel(px, py, currentDrawColor);
}

void ott::ImageBuffer::BlendPixel(const unsigned short& px, const unsigned short& py, const ColorRGB& color) {
	LogicalColor pixel(nChannels >= 4);
	if (!this->GetPixel(px, py, pixel)) // Invalid pixel position
		return;
	this->BlendPixel(pixel, color);
}

void ott::ImageBuffer::BlendPixel(LogicalColor& dest, const ColorRGB& src) const {
	switch (colorBlendMode) {
	case BlendMode::None:
		break;
	case BlendMode::Normal:
		dest.Set(src);
		break;
	case BlendMode::Add:
		dest += src;
		break;
	case BlendMode::Sub:
		dest -= src;
		break;
	case BlendMode::Mult:
		dest *= src;
		break;
	case BlendMode::Divide:
		dest /= src;
		break;
	case BlendMode::Diff:
		dest.Difference(src);
		break;
	case BlendMode::Darken:
		dest.Darken(src);
		break;
	case BlendMode::Lighten:
		dest.Lighten(src);
		break;
	case BlendMode::Average:
		dest.Average(src);
		break;
	case BlendMode::Replace:
		dest.Set(src);
		break;
	default: // Unknown blending mode. Do nothing
		break;
	}
}

void ott::ImageBuffer::BlendPixel(LogicalColor& dest, const LogicalColor& src) const {
	switch (colorBlendMode) {
	case BlendMode::None:
		break;
	case BlendMode::Normal:
		dest.Set(src);
		break;
	case BlendMode::Add:
		dest += src;
		break;
	case BlendMode::Sub:
		dest -= src;
		break;
	case BlendMode::Mult:
		dest *= src;
		break;
	case BlendMode::Divide:
		dest /= src;
		break;
	case BlendMode::Diff:
		dest.Difference(src);
		break;
	case BlendMode::Darken:
		dest.Darken(src);
		break;
	case BlendMode::Lighten:
		dest.Lighten(src);
		break;
	case BlendMode::Average:
		dest.Average(src);
		break;
	case BlendMode::Replace:
		dest.Set(src);
		break;
	default: // Unknown blending mode. Do nothing
		break;
	}
}

