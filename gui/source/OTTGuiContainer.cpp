#include "OTTGuiContainer.hpp"
#include "OTTGuiElement.hpp"
#include "OTTImageBuffer.hpp"
#include "OTTMouse.hpp"

OTTGuiContainer::OTTGuiContainer() :
	OTTGuiFrame()
{
}

OTTGuiContainer::OTTGuiContainer(const int& x, const int& y, const int& w, const int& h) :
	OTTGuiFrame(x, y, w, h)
{
}

bool OTTGuiContainer::handleMouseEvents(OTTMouse* mouse) {
	bool retval = false;
	//bool prevMouseState = bMouseInside;
	bMouseInside = isInside((int)mouse->getX(), (int)mouse->getY());
	if (bMouseInside) { // Check for clicks on top of components
		for (auto ch = children.begin(); ch != children.end(); ch++)
			(*ch)->update(mouse);
	}
	return retval;
}

OTTGuiElement* OTTGuiContainer::add(OTTGuiElement* child) {
	child->setPositionOffset(x0, y0);
	children.push_back(child);
	return child;
}

void OTTGuiContainer::draw(OTTImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	buffer->setDrawColor(Colors::RED);
	buffer->drawRectangle(x0, y0, x1, y1);
	for (auto ch = children.begin(); ch != children.end(); ch++)
		(*ch)->draw(buffer);
}
