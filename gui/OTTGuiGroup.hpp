#ifndef OTT_GUI_GROUP_HPP
#define OTT_GUI_GROUP_HPP

#include <vector>

class OTTGuiElement;

class OTTGuiGroup {
public:
	OTTGuiGroup() :
		elements()
	{
	}

	void add(OTTGuiElement* obj) {
		elements.push_back(obj);
	}

	std::vector<OTTGuiElement*>::iterator begin() {
		return elements.begin();
	}

	std::vector<OTTGuiElement*>::iterator end() {
		return elements.end();
	}

protected:
	std::vector<OTTGuiElement*> elements;
};

#endif // ifndef OTT_GUI_GROUP_HPP
