#include <iostream> // TEMP

#include "OTTGuiCheckbox.hpp"
#include "OTTImageBuffer.hpp"

OTTGuiCheckbox::OTTGuiCheckbox() :
	OTTGuiElement()
{
}

void OTTGuiCheckbox::draw(OTTImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	if (!bState) {
		buffer->setDrawColor(backgroundColor);
		buffer->drawRectangle(
			x0 + xOffset,
			y0 + yOffset,
			x1 + xOffset,
			y1 + yOffset
		);
		//buffer->drawSubImage(x0 + xOffset, y0 + yOffset, normalStateImage);
	}
	else {
		buffer->setDrawColor(backgroundColor);
		buffer->drawRectangle(
			x0 + xOffset,
			y0 + yOffset,
			x1 + xOffset,
			y1 + yOffset
		);
		buffer->drawLine(x0 + xOffset, y0 + yOffset, x1 + xOffset, y1 + yOffset);
		buffer->drawLine(x0 + xOffset, y1 + yOffset, x1 + xOffset, y0 + yOffset);
		//buffer->drawSubImage(x0 + xOffset, y0 + yOffset, activeStateImage);
	}
}

bool OTTGuiCheckbox::onUserMouseEnter(const int& x, const int& y) {
	return false;
}

bool OTTGuiCheckbox::onUserMouseExit(const int& x, const int& y) {
	return false;
}

bool OTTGuiCheckbox::onUserMousePressed(const int& x, const int& y, const unsigned char& buttons) {
	bState = !bState;
	if (onStateChanged) {
		(*onStateChanged)(bState);
	}
	return bState;
}

bool OTTGuiCheckbox::onUserMouseReleased(const int& x, const int& y, const unsigned char& buttons) {
	return bState;
}