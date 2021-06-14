#ifndef OTT_GUI_ELEMENT_HPP
#define OTT_GUI_ELEMENT_HPP

#include "OTTGuiFrame.hpp"
#include "ColorRGB.hpp"

class OTTMouse;

class OTTGuiElement : public OTTGuiFrame {
public:
	typedef void (*buttonAction)(OTTGuiElement*, bool);

	typedef void (*stateChanged)(OTTGuiElement*, bool);

	OTTGuiElement();

	bool isEnabled() const {
		return bEnabled;
	}

	bool getState() const {
		return bState;
	}

	void enable() {
		bEnabled = true;
	}

	void disable() {
		bEnabled = false;
	}

	void setState(bool state) {
		bState = state;
	}

	void setPositionOffset(const int& offsetX, const int& offsetY) {
		xOffset = offsetX;
		yOffset = offsetY;
	}

	void setLeftMouseButtonPressed(buttonAction action) {
		onButtonPressed[0] = action;
	}

	void setRightMouseButtonPressed(buttonAction action) {
		onButtonPressed[1] = action;
	}

	void setMiddleMouseButtonPressed(buttonAction action) {
		onButtonPressed[2] = action;
	}

	void setLeftMouseButtonReleased(buttonAction action) {
		onButtonReleased[0] = action;
	}

	void setRightMouseButtonReleased(buttonAction action) {
		onButtonReleased[1] = action;
	}

	void setMiddleMouseButtonReleased(buttonAction action) {
		onButtonReleased[2] = action;
	}

	void setOnStateChanged(stateChanged action) {
		onStateChanged = action;
	}

	void setOnEditingFinished(stateChanged action) {
		onEditingFinished = action;
	}

	void setNormalStateImage(OTTImageBuffer* image) {
		normalStateImage = image;
	}

	void setActiveStateImage(OTTImageBuffer* image) {
		activeStateImage = image;
	}

	void setDisabledStateImage(OTTImageBuffer* image) {
		disabledStateImage = image;
	}

	bool update(OTTMouse* mouse);

	void draw(OTTImageBuffer* buffer) override;

protected:
	std::string sName;

	bool bEnabled;

	bool bState;

	int xOffset; ///< Horizontal pixel offset inside the parent container

	int yOffset; ///< Vertical pixel offset inside the parent container

	ColorRGB foregroundColor;

	ColorRGB backgroundColor;

	bool bPressed[3];

	buttonAction onButtonPressed[3];

	buttonAction onButtonReleased[3];

	stateChanged onStateChanged;

	stateChanged onEditingFinished;

	OTTImageBuffer* normalStateImage;

	OTTImageBuffer* activeStateImage;

	OTTImageBuffer* disabledStateImage;

	virtual void updateMouse(const int&, const int&) {
	}

	virtual bool onUserMouseEnter(const int&, const int&) {
		return false;
	}

	virtual bool onUserMouseExit(const int&, const int&) {
		return false;
	}

	virtual bool onUserMousePressed(const int&, const int&, const unsigned char&) {
		return false;
	}

	virtual bool onUserMouseReleased(const int&, const int&, const unsigned char&) {
		return false;
	}
};

#endif // ifndef OTT_GUI_ELEMENT_HPP
