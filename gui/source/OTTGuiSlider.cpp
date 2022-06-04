#include "OTTGuiSlider.hpp"

#include <graphics/core/OTTImageBuffer.hpp>

void ott::GuiSlider::Draw(ImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	if (bState == false) {
		buffer->SetDrawColor(colors::Violet);
	}
	else {
		buffer->SetDrawColor(colors::Magenta);
	}
	buffer->DrawRectangle(
		x0 + xOffset,
		y0 + yOffset,
		x1 + xOffset,
		y1 + yOffset
	);
	if (bHorizontal) {
		int32_t lineX = (int32_t)(fValue * nWidth);
		buffer->DrawRectangle(
			x0 + xOffset,
			y0 + yOffset,
			x0 + xOffset + lineX,
			y1 + yOffset,
			true
		);
	}
	else {
		int32_t lineY = (int32_t)(fValue * nHeight);
		buffer->DrawLine(
			x0 + xOffset,
			y0 + yOffset,
			x1 + xOffset,
			y0 + yOffset + lineY
		);
	}
}

void ott::GuiSlider::UpdateMouse(const int32_t& x, const int32_t& y) {
	if (bHorizontal) {
		fValue = 1.f * x / nWidth; // 0 to 1
	}
	else {
		fValue = 1.f * y / nHeight; // 0 to 1
	}
}

bool ott::GuiSlider::OnUserMouseEnter(const int32_t& x, const int32_t& y) {
	return false;
}

bool ott::GuiSlider::OnUserMouseExit(const int32_t& x, const int32_t& y) {
	if (bState && onEditingFinished) {
		(*onEditingFinished)(this, true);
	}
	bState = false;
	return false;
}

bool ott::GuiSlider::OnUserMousePressed(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	bState = true;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	return true;
}

bool ott::GuiSlider::OnUserMouseReleased(const int32_t& x, const int32_t& y, const uint8_t& buttons) {
	bState = false;
	if (onStateChanged) {
		(*onStateChanged)(this, bState);
	}
	if (onEditingFinished) {
		(*onEditingFinished)(this, true);
	}
	return false;
}
