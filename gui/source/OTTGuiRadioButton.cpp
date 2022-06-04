#include "OTTGuiRadioButton.hpp"

#include <graphics/core/OTTImageBuffer.hpp>

ott::GuiRadioButton::GuiRadioButton() :
	GuiElement()
{
}

void ott::GuiRadioButton::Draw(ImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	if (bState == false) {
		if (normalStateImage == false) {
			buffer->SetDrawColor(colors::Green);
		}
		else {
			buffer->DrawSubImage(x0 + xOffset, y0 + yOffset, normalStateImage);
		}
	}
	else {
		if (activeStateImage == false) {
			buffer->SetDrawColor(colors::Yellow);
		}
		else {
			buffer->DrawSubImage(x0 + xOffset, y0 + yOffset, activeStateImage);
		}
	}
	buffer->DrawCircle(
		x0 + xOffset,
		y0 + yOffset,
		nWidth / 2.f);
	buffer->DrawCircle(
		x0 + xOffset,
		y0 + yOffset,
		0.75f * nWidth / 2.f,
		true
	);
}

bool ott::GuiRadioButton::OnUserMouseEnter(const int32_t& x, const int32_t& y) {
	return false;
}

bool ott::GuiRadioButton::OnUserMouseExit(const int32_t& x, const int32_t& y) {
	bState = false;
	return false;
}

bool ott::GuiRadioButton::OnUserMousePressed(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	bState = true;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return true;
}

bool ott::GuiRadioButton::OnUserMouseReleased(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	bState = false;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return false;
}