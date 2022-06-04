#ifndef GUI_OTT_GUI_ELEMENT_HPP
#define GUI_OTT_GUI_ELEMENT_HPP

#include "OTTGuiFrame.hpp"

#include <graphics/core/OTTColor.hpp>

#include <string>

namespace ott {

class Mouse;

class GuiElement : public GuiFrame {
public:

	typedef void (*buttonAction)(GuiElement*, bool);
	typedef void (*stateChanged)(GuiElement*, bool);

	GuiElement();

	bool Enabled() const {
		return bEnabled;
	}

	bool State() const {
		return bState;
	}

	void Enable() {
		bEnabled = true;
	}

	void Disable() {
		bEnabled = false;
	}

	void SetState(bool State) {
		bState = State;
	}

	void SetPositionOffset(const int32_t& offsetX, const int32_t& offsetY) {
		xOffset = offsetX;
		yOffset = offsetY;
	}

	void SetLeftMouseButtonPressed(buttonAction action) {
		onButtonPressed[0] = action;
	}

	void SetRightMouseButtonPressed(buttonAction action) {
		onButtonPressed[1] = action;
	}

	void SetMiddleMouseButtonPressed(buttonAction action) {
		onButtonPressed[2] = action;
	}

	void SetLeftMouseButtonReleased(buttonAction action) {
		onButtonReleased[0] = action;
	}

	void SetRightMouseButtonReleased(buttonAction action) {
		onButtonReleased[1] = action;
	}

	void SetMiddleMouseButtonReleased(buttonAction action) {
		onButtonReleased[2] = action;
	}

	void SetOnStateChanged(stateChanged action) {
		onStateChanged = action;
	}

	void SetOnEditingFinished(stateChanged action) {
		onEditingFinished = action;
	}

	void SetNormalStateImage(ImageBuffer* image) {
		normalStateImage = image;
	}

	void SetActiveStateImage(ImageBuffer* image) {
		activeStateImage = image;
	}

	void SetDisabledStateImage(ImageBuffer* image) {
		disabledStateImage = image;
	}

	bool Update(Mouse* mouse);

	void Draw(ImageBuffer* buffer) override;

protected:

	std::string sName;

	bool bEnabled;
	bool bState;

	int32_t xOffset; ///< Horizontal pixel offset inside the parent container
	int32_t yOffset; ///< Vertical pixel offset inside the parent container

	ColorRGB foregroundColor;
	ColorRGB backgroundColor;

	bool bPressed[3];

	buttonAction onButtonPressed[3];
	buttonAction onButtonReleased[3];

	stateChanged onStateChanged;
	stateChanged onEditingFinished;

	ImageBuffer* normalStateImage;
	ImageBuffer* activeStateImage;
	ImageBuffer* disabledStateImage;

	virtual void UpdateMouse(const int32_t&, const int32_t&) {
	}

	virtual bool OnUserMouseEnter(const int32_t&, const int32_t&) {
		return false;
	}

	virtual bool OnUserMouseExit(const int32_t&, const int32_t&) {
		return false;
	}

	virtual bool OnUserMousePressed(const int32_t&, const int32_t&, const uint8_t&) {
		return false;
	}

	virtual bool OnUserMouseReleased(const int32_t&, const int32_t&, const uint8_t&) {
		return false;
	}
};

} /* namespace ott */

#endif // #ifndef GUI_OTT_GUI_ELEMENT_HPP
