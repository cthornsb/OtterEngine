#include "OTTGuiSlider.hpp"
#include "OTTImageBuffer.hpp"

OTTGuiSlider::OTTGuiSlider() :
	OTTGuiElement(),
	bHorizontal(true),
	fValue(0.f),
	fMin(0.f),
	fMax(1.f)
{
}

void OTTGuiSlider::draw(OTTImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	if (!bState)
		buffer->setDrawColor(Colors::VIOLET);
	else
		buffer->setDrawColor(Colors::MAGENTA);
	buffer->drawRectangle(
		x0 + xOffset,
		y0 + yOffset,
		x1 + xOffset,
		y1 + yOffset
	);
	if (bHorizontal) {
		int lineX = (int)(fValue * nWidth);
		buffer->drawRectangle(
			x0 + xOffset,
			y0 + yOffset,
			x0 + xOffset + lineX,
			y1 + yOffset,
			true
		);
	}
	else {
		int lineY = (int)(fValue * nHeight);
		buffer->drawLine(
			x0 + xOffset,
			y0 + yOffset,
			x1 + xOffset,
			y0 + yOffset + lineY
		);
	}
}

void OTTGuiSlider::updateMouse(const int& x, const int& y) {
	if(bHorizontal)
		fValue = 1.f * x / nWidth; // 0 to 1
	else
		fValue = 1.f * y / nHeight; // 0 to 1
}

bool OTTGuiSlider::onUserMouseEnter(const int& x, const int& y) {
	return false;
}

bool OTTGuiSlider::onUserMouseExit(const int& x, const int& y) {
	if (bState && onEditingFinished) {
		(*onEditingFinished)(true);
	}
	bState = false;
	return false;
}

bool OTTGuiSlider::onUserMousePressed(const int& x, const int& y, const unsigned char& buttons) {
	bState = true;
	if (onStateChanged) {
		(*onStateChanged)(bState);
	}
	return true;
}

bool OTTGuiSlider::onUserMouseReleased(const int& x, const int& y, const unsigned char& buttons) {
	bState = false;
	if (onStateChanged) {
		(*onStateChanged)(bState);
	}
	if (onEditingFinished) {
		(*onEditingFinished)(true);
	}
	return false;
}