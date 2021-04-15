#ifndef OTT_GUI_CONTAINER_HPP
#define OTT_GUI_CONTAINER_HPP

#include <memory>

#include "OTTGuiFrame.hpp"
#include "OTTGuiElement.hpp"

class OTTMouse;

class OTTGuiContainer : public OTTGuiFrame {
public:
	OTTGuiContainer();

	OTTGuiContainer(const int& x, const int& y, const int& w, const int& h);

	bool handleMouseEvents(OTTMouse* mouse);

	OTTGuiElement* add(OTTGuiElement* child);

	void draw(OTTImageBuffer* buffer) override;

protected:
	std::vector<std::unique_ptr<OTTGuiElement> > children;
};

#endif // ifndef OTT_GUI_CONTAINER_HPP
