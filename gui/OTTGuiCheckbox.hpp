#ifndef GUI_OTT_GUI_CHECKBOX_HPP
#define GUI_OTT_GUI_CHECKBOX_HPP

#include "OTTGuiElement.hpp"

namespace ott {

class GuiCheckbox : public GuiElement {
public:

	GuiCheckbox();

	void Draw(ImageBuffer*) override;

protected:

	bool OnUserMouseEnter(const int32_t& x, const int32_t& y) override;

	bool OnUserMouseExit(const int32_t& x, const int32_t& y) override;

	bool OnUserMousePressed(const int32_t& x, const int32_t& y, const uint8_t& buttons) override;

	bool OnUserMouseReleased(const int32_t& x, const int32_t& y, const uint8_t& buttons) override;
};

} /* namespace ott */

#endif // #ifndef GUI_OTT_GUI_CHECKBOX_HPP
