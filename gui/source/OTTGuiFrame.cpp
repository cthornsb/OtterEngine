#include "OTTGuiFrame.hpp"

#include <graphics/core/OTTImageBuffer.hpp>

ott::GuiFrame::GuiFrame() :
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

ott::GuiFrame::GuiFrame(const int32_t& x, const int32_t& y, const int32_t& w, const int32_t& h) :
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

void ott::GuiFrame::SetPosition(const int32_t& x, const int32_t& y) {
	x0 = x;
	y0 = y;
	x1 = x + nWidth;
	y1 = y + nHeight;
}

void ott::GuiFrame::SetSize(const int32_t& w, const int32_t& h) {
	nWidth = w;
	nHeight = h;
	x1 = x0 + nWidth;
	y1 = y0 + nHeight;
}

void ott::GuiFrame::Draw(ImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	buffer->DrawRectangle(x0, y0, x1, y1);
}
