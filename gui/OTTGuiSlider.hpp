#ifndef GUI_OTT_GUI_SLIDER_HPP
#define GUI_OTT_GUI_SLIDER_HPP

#include "OTTGuiElement.hpp"

namespace ott {

class GuiSlider : public GuiElement {
public:

	/** Default constructor (range 0 to 1)
	  */
	GuiSlider() :
		GuiElement(),
		bHorizontal(true),
		fValue(0.f),
		fMin(0.f),
		fMax(1.f)
	{
	}
	
	/** Slider range and initial value constructor
	  */
	GuiSlider(const float& value, const float& minValue, const float& maxValue, bool horizontal = true) : 
		GuiElement(),
		bHorizontal(horizontal),
		fValue(value),
		fMin(minValue),
		fMax(maxValue)
	{
	}

	/** Get the current slider value
	  */
	float Value() const {
		return (fMin + (fMax - fMin) * fValue);
	}

	/** Get the current slider position as a fraction of its full range
	  */
	float Fraction() const {
		return fValue;
	}
	
	/** Set the current slider value
	  */
	void SetValue(const float& val) {
		fValue = (val - fMin) / (fMax - fMin);
	}

	/** Set the current slider position as a fraction of its full range
	  */
	void SetFraction(const float& val) {
		fValue = val;
	}

	void Draw(ImageBuffer*) override;

protected:

	bool bHorizontal; ///< Set if slider slides horizontally, otherwise it slides vertically

	float fValue; ///< Current slider value between 0 and 1
	float fMin; ///< Minimum slider value
	float fMax; ///< Maximum slider value

	void UpdateMouse(const int32_t& x, const int32_t& y) override;

	bool OnUserMouseEnter(const int32_t& x, const int32_t& y) override;

	bool OnUserMouseExit(const int32_t& x, const int32_t& y) override;

	bool OnUserMousePressed(const int32_t& x, const int32_t& y, const uint8_t& buttons) override;

	bool OnUserMouseReleased(const int32_t& x, const int32_t& y, const uint8_t& buttons) override;
};

} /* namespace ott */

#endif // #ifndef GUI_OTT_GUI_SLIDER_HPP
