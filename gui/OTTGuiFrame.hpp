#ifndef GUI_OTT_GUI_FRAME_HPP
#define GUI_OTT_GUI_FRAME_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace ott {

class ImageBuffer;

class GuiFrame {
public:

	GuiFrame();

	GuiFrame(const int32_t& x, const int32_t& y, const int32_t& w, const int32_t& h);

	bool Inside(const int32_t& x, const int32_t& y) const {
		return (x >= x0 && x <= x1 && y >= y0 && y <= y1);
	}

	void SetPosition(const int32_t& x, const int32_t& y);

	void SetSize(const int32_t& w, const int32_t& h);

	virtual void Draw(ImageBuffer* buffer);

protected:

	bool bMouseInside;
	bool bDebugMode;

	int32_t x0;
	int32_t y0;
	int32_t x1;
	int32_t y1;

	int32_t nWidth;
	int32_t nHeight;
};

} /* namespace ott */

#endif // #ifndef GUI_OTT_GUI_FRAME_HPP
