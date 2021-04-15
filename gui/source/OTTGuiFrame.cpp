#include "OTTImageBuffer.hpp"
#include "OTTGuiFrame.hpp"

OTTGuiFrame::OTTGuiFrame() :
	bMouseInside(false),
	bDebugMode(false),
	x0(0),
	y0(0),
	x1(0),
	y1(0),
	nWidth(0),
	nHeight(0)
{
}

OTTGuiFrame::OTTGuiFrame(const int& x, const int& y, const int& w, const int& h) :
	bMouseInside(false),
	bDebugMode(false),
	x0(x),
	y0(y),
	x1(x + w),
	y1(y + h),
	nWidth(w),
	nHeight(h)
{
}

void OTTGuiFrame::setPosition(const int& x, const int& y) {
	x0 = x;
	y0 = y;
	x1 = x + nWidth;
	y1 = y + nHeight;
}

void OTTGuiFrame::setSize(const int& w, const int& h) {
	nWidth = w;
	nHeight = h;
	x1 = x0 + nWidth;
	y1 = y0 + nHeight;
}

void OTTGuiFrame::draw(OTTImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	buffer->drawRectangle(x0, y0, x1, y1);
}
