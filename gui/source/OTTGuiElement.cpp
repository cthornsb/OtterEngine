#include "OTTGuiElement.hpp"
#include "OTTImageBuffer.hpp"
#include "OTTMouse.hpp"

OTTGuiElement::OTTGuiElement() :
	OTTGuiFrame(),
	sName("element"),
	bEnabled(true),
	bState(false),
	xOffset(0),
	yOffset(0),
	foregroundColor(Colors::BLACK),
	backgroundColor(Colors::WHITE),
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

bool OTTGuiElement::update(OTTMouse* mouse) {
	if (!bEnabled)
		return false;
	unsigned char buttons = 0;
	buttons |= (mouse->check(0) ? 1 : 0);
	buttons |= (mouse->check(1) ? 2 : 0);
	buttons |= (mouse->check(2) ? 4 : 0);
	int x = (int)mouse->getX() - xOffset;
	int y = (int)mouse->getY() - yOffset;
	bool prevMouseState = bMouseInside;
	bMouseInside = isInside(x, y);
	if (!prevMouseState && bMouseInside) { // Mouse entered element
		this->onUserMouseEnter(x, y);
	}
	else if (prevMouseState && !bMouseInside) { // Mouse exited element
		this->onUserMouseExit(x, y);
	}
	if (bMouseInside) { // Update button states
		for (unsigned char i = 0; i < 3; i++) {
			if (!bPressed[i] && ((buttons & (0x1 << i)) == 0x1)) { // Button was just clicked on
				bPressed[i] = true;
				if (onButtonPressed[i])
					(*onButtonPressed[i])(true);
				this->onUserMousePressed(x - x0, y - y0, buttons);
			}
			if (bPressed[i] && ((buttons & (0x1 << i)) == 0x0)) { // Button was just released
				bPressed[i] = false;
				if (onButtonReleased[i])
					(*onButtonReleased[i])(false);
				this->onUserMouseReleased(x - x0, y - y0, buttons);
			}
			if (bPressed[i]) {
				this->updateMouse(x - x0, y - y0);
			}
		}		
	}
	return bMouseInside;
}

void OTTGuiElement::draw(OTTImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	buffer->setDrawColor(backgroundColor);
	buffer->drawRectangle(
		x0 + xOffset, 
		y0 + yOffset, 
		x1 + xOffset,
		y1 + yOffset
	);
}
