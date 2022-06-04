#include "OTTGuiElement.hpp"

#include <graphics/core/OTTImageBuffer.hpp>
#include <input/OTTMouse.hpp>

ott::GuiElement::GuiElement() :
	GuiFrame(),
	sName("element"),
	bEnabled(true),
	bState(false),
	xOffset(0),
	yOffset(0),
	foregroundColor(colors::Black),
	backgroundColor(colors::White),
	bPressed{ 0, 0, 0 },
	onButtonPressed{ 0x0, 0x0, 0x0 },
	onButtonReleased{ 0x0, 0x0, 0x0 },
	onStateChanged(0x0),
	onEditingFinished(0x0),
	normalStateImage(0x0),
	activeStateImage(0x0),
	disabledStateImage(0x0)
{
}

bool ott::GuiElement::Update(Mouse* mouse) {
	if (!bEnabled)
		return false;
	uint8_t buttons = 0;
	buttons |= (mouse->Check(0) ? 1 : 0);
	buttons |= (mouse->Check(1) ? 2 : 0);
	buttons |= (mouse->Check(2) ? 4 : 0);
	int32_t x = (int32_t)mouse->X() - xOffset;
	int32_t y = (int32_t)mouse->Y() - yOffset;
	bool prevMouseState = bMouseInside;
	bMouseInside = this->Inside(x, y);
	if (!prevMouseState && bMouseInside) { // Mouse entered element
		this->OnUserMouseEnter(x, y);
	}
	else if (prevMouseState && !bMouseInside) { // Mouse exited element
		this->OnUserMouseExit(x, y);
	}
	if (bMouseInside) { // Update button states
		for (uint8_t i = 0; i < 3; i++) {
			if (!bPressed[i] && ((buttons & (0x1 << i)) == 0x1)) { // Button was just clicked on
				bPressed[i] = true;
				if (onButtonPressed[i])
					(*onButtonPressed[i])(this, true);
				this->OnUserMousePressed(x - x0, y - y0, buttons);
			}
			if (bPressed[i] && ((buttons & (0x1 << i)) == 0x0)) { // Button was just released
				bPressed[i] = false;
				if (onButtonReleased[i])
					(*onButtonReleased[i])(this, false);
				this->OnUserMouseReleased(x - x0, y - y0, buttons);
			}
			if (bPressed[i]) {
				this->UpdateMouse(x - x0, y - y0);
			}
		}		
	}
	return bMouseInside;
}

void ott::GuiElement::Draw(ImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	buffer->SetDrawColor(backgroundColor);
	buffer->DrawRectangle(
		x0 + xOffset, 
		y0 + yOffset, 
		x1 + xOffset,
		y1 + yOffset
	);
}
