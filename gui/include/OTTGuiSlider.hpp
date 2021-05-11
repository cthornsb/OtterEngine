#ifndef OTT_GUI_SLIDER_HPP
#define OTT_GUI_SLIDER_HPP

#include "OTTGuiElement.hpp"

class OTTGuiSlider : public OTTGuiElement {
public:
	/** Default constructor (range 0 to 1)
	  */
	OTTGuiSlider() :
		OTTGuiElement(),
		bHorizontal(true),
		fValue(0.f),
		fMin(0.f),
		fMax(1.f)
	{
	}
	
	/** Slider range and initial value constructor
	  */
	OTTGuiSlider(const float& value, const float& minValue, const float& maxValue, bool horizontal = true) : 
		OTTGuiElement(),
		bHorizontal(horizontal),
		fValue(value),
		fMin(minValue),
		fMax(maxValue)
	{
	}

	/** Get the current slider value
	  */
	float getValue() const {
		return (fMin + (fMax - fMin) * fValue);
	}

	/** Get the current slider position as a fraction of its full range
	  */
	float getFraction() const {
		return fValue;
	}
	
	/** Set the current slider value
	  */
	void setValue(const float& val) {
		fValue = (val - fMin) / (fMax - fMin);
	}

	/** Set the current slider position as a fraction of its full range
	  */
	void setFraction(const float& val) {
		fValue = val;
	}

	void draw(OTTImageBuffer*) override;

protected:
	bool bHorizontal; ///< Set if slider slides horizontally, otherwise it slides vertically

	float fValue; ///< Current slider value between 0 and 1

	float fMin; ///< Minimum slider value

	float fMax; ///< Maximum slider value

	void updateMouse(const int& x, const int& y) override;

	bool onUserMouseEnter(const int& x, const int& y) override;

	bool onUserMouseExit(const int& x, const int& y) override;

	bool onUserMousePressed(const int& x, const int& y, const unsigned char& buttons) override;

	bool onUserMouseReleased(const int& x, const int& y, const unsigned char& buttons) override;
};

#endif // ifndef OTT_GUI_SLIDER_HPP
