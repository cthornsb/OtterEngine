#ifndef OTT_GUI_BUTTON_HPP
#define OTT_GUI_BUTTON_HPP

#include "OTTGuiElement.hpp"

class OTTGuiButton : public OTTGuiElement {
public:
	OTTGuiButton();

	void draw(OTTImageBuffer*) override;

protected:
	bool onUserMouseEnter(const int& x, const int& y) override;

	bool onUserMouseExit(const int& x, const int& y) override;

	bool onUserMousePressed(const int& x, const int& y, const unsigned char& buttons) override;

	bool onUserMouseReleased(const int& x, const int& y, const unsigned char& buttons) override;
};

#endif // ifndef OTT_GUI_BUTTON_HPP