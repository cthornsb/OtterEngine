#ifndef GUI_OTT_GUI_CONTAINER_HPP
#define GUI_OTT_GUI_CONTAINER_HPP

#include "OTTGuiFrame.hpp"

namespace ott {

class GuiElement;
class Mouse;

class GuiContainer : public GuiFrame {
public:

	GuiContainer();

	GuiContainer(const int32_t& x, const int32_t& y, const int32_t& w, const int32_t& h);

	bool HandleMouseEvents(Mouse* mouse);

	GuiElement* Add(GuiElement* child);

	void Draw(ImageBuffer* buffer) override;

protected:

	std::vector<GuiElement*> children;
};

} /* namespace ott */

#endif // #ifndef GUI__GUI_CONTAINER_HPP
