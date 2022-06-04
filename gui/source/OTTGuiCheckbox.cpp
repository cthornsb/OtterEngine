#include "OTTGuiCheckbox.hpp"

#include <graphics/core/OTTImageBuffer.hpp>

ott::GuiCheckbox::GuiCheckbox() :
	GuiElement()
{
}

void ott::GuiCheckbox::Draw(ImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	if (!bState) {
		buffer->SetDrawColor(backgroundColor);
		buffer->DrawRectangle(
			x0 + xOffset,
			y0 + yOffset,
			x1 + xOffset,
			y1 + yOffset
		);
		//buffer->drawSubImage(x0 + xOffset, y0 + yOffset, normalStateImage);
	}
	else {
		buffer->SetDrawColor(backgroundColor);
		buffer->DrawRectangle(
			x0 + xOffset,
			y0 + yOffset,
			x1 + xOffset,
			y1 + yOffset
		);
		buffer->DrawLine(x0 + xOffset, y0 + yOffset, x1 + xOffset, y1 + yOffset);
		buffer->DrawLine(x0 + xOffset, y1 + yOffset, x1 + xOffset, y0 + yOffset);
		//buffer->drawSubImage(x0 + xOffset, y0 + yOffset, activeStateImage);
	}
}

bool ott::GuiCheckbox::OnUserMouseEnter(const int32_t& x, const int32_t& y) {
	return false;
}

bool ott::GuiCheckbox::OnUserMouseExit(const int32_t& x, const int32_t& y) {
	return false;
}

bool ott::GuiCheckbox::OnUserMousePressed(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	bState = !bState;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return bState;
}

bool ott::GuiCheckbox::OnUserMouseReleased(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	return bState;
}