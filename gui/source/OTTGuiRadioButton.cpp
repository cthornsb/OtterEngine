#include "OTTGuiRadioButton.hpp"
#include "OTTImageBuffer.hpp"

OTTGuiRadioButton::OTTGuiRadioButton() :
	OTTGuiElement()
{
}

void OTTGuiRadioButton::draw(OTTImageBuffer* buffer) {
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
	buffer->drawCircle(
		x0 + xOffset,
		y0 + yOffset,
		nWidth / 2.f);
	buffer->drawCircle(
		x0 + xOffset,
		y0 + yOffset,
		0.75f * nWidth / 2.f,
		true
	);
}

bool OTTGuiRadioButton::onUserMouseEnter(const int& x, const int& y) {
	return false;
}

bool OTTGuiRadioButton::onUserMouseExit(const int& x, const int& y) {
	bState = false;
	return false;
}

bool OTTGuiRadioButton::onUserMousePressed(const int& x, const int& y, const unsigned char& buttons) {
	bState = true;
	if (onStateChanged) {
		(*onStateChanged)(bState);
	}
	return true;
}

bool OTTGuiRadioButton::onUserMouseReleased(const int& x, const int& y, const unsigned char& buttons) {
	bState = false;
	if (onStateChanged) {
		(*onStateChanged)(bState);
	}
	return false;
}