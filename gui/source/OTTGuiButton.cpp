#include "OTTGuiButton.hpp"

#include <graphics/core/OTTImageBuffer.hpp>

ott::GuiButton::GuiButton() :
	GuiElement()
{
}

void ott::GuiButton::Draw(ImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	if (!bState) {
		if(!normalStateImage)
			buffer->SetDrawColor(colors::Green);
		else
		buffer->DrawSubImage(x0 + xOffset, y0 + yOffset, normalStateImage);
	}
	else {
		if(!activeStateImage)
			buffer->SetDrawColor(colors::Yellow);
		else
			buffer->DrawSubImage(x0 + xOffset, y0 + yOffset, activeStateImage);
	}
	buffer->DrawRectangle(
		x0 + xOffset,
		y0 + yOffset,
		x0 + xOffset + nWidth,
		y0 + yOffset + nHeight,
		true
	);
}

bool ott::GuiButton::OnUserMouseEnter(const int32_t& x, const int32_t& y) {
	return false;
}

bool ott::GuiButton::OnUserMouseExit(const int32_t& x, const int32_t& y) {
	bState = false;
	return false;
}

bool ott::GuiButton::OnUserMousePressed(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	bState = true;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return true;
}

bool ott::GuiButton::OnUserMouseReleased(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	bState = false;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return false;
}
