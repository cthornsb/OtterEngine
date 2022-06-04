#include "OTTGuiContainer.hpp"
#include "OTTGuiElement.hpp"

#include <graphics/core/OTTImageBuffer.hpp>
#include <input/OTTMouse.hpp>

ott::GuiContainer::GuiContainer() :
	GuiFrame()
{
}

ott::GuiContainer::GuiContainer(const int32_t& x, const int32_t& y, const int32_t& w, const int32_t& h) :
	GuiFrame(x, y, w, h)
{
}

bool ott::GuiContainer::HandleMouseEvents(Mouse* mouse) {
	bool retval = false;
	//bool prevMouseState = bMouseInside;
	bMouseInside = this->Inside((int32_t)mouse->X(), (int32_t)mouse->Y());
	if (bMouseInside) { // Check for clicks on top of components
		for (auto ch = children.begin(); ch != children.end(); ch++)
			(*ch)->Update(mouse);
	}
	return retval;
}

ott::GuiElement* ott::GuiContainer::Add(GuiElement* child) {
	child->SetPositionOffset(x0, y0);
	children.push_back(child);
	return child;
}

void ott::GuiContainer::Draw(ImageBuffer* buffer) {
	// Draw a simple wireframe bounding box, for debugging
	buffer->SetDrawColor(colors::Red);
	buffer->DrawRectangle(x0, y0, x1, y1);
	for (auto ch = children.begin(); ch != children.end(); ch++)
		(*ch)->Draw(buffer);
}
