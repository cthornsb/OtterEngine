#include "OTTGuiButton.hpp"
#include "OTTImageBuffer.hpp"

OTTGuiButton::OTTGuiButton() :
	OTTGuiElement()
{
}

void OTTGuiButton::draw(OTTImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	if (!bState) {
		if(!normalStateImage)
			buffer->setDrawColor(Colors::GREEN);
		else
		buffer->drawSubImage(x0 + xOffset, y0 + yOffset, normalStateImage);
	}
	else {
		if(!activeStateImage)
			buffer->setDrawColor(Colors::YELLOW);
		else
			buffer->drawSubImage(x0 + xOffset, y0 + yOffset, activeStateImage);
	}
	buffer->drawRectangle(
		x0 + xOffset,
		y0 + yOffset,
		x0 + xOffset + nWidth,
		y0 + yOffset + nHeight,
		true
	);
}

bool OTTGuiButton::onUserMouseEnter(const int& x, const int& y) {
	return false;
}

bool OTTGuiButton::onUserMouseExit(const int& x, const int& y) {
	bState = false;
	return false;
}

bool OTTGuiButton::onUserMousePressed(const int& x, const int& y, const unsigned char& buttons) {
	bState = true;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return true;
}

bool OTTGuiButton::onUserMouseReleased(const int& x, const int& y, const unsigned char& buttons) {
	bState = false;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return false;
}