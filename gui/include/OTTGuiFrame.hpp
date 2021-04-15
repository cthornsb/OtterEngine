#ifndef OTT_GUI_FRAME_HPP
#define OTT_GUI_FRAME_HPP

#include <vector>
#include <string>

class OTTImageBuffer;

class OTTGuiFrame {
public:
	OTTGuiFrame();

	OTTGuiFrame(const int& x, const int& y, const int& w, const int& h);

	bool isInside(const int& x, const int& y) const {
		return (x >= x0 && x <= x1 && y >= y0 && y <= y1);
	}

	void setPosition(const int& x, const int& y);

	void setSize(const int& w, const int& h);

	virtual void draw(OTTImageBuffer* buffer);

protected:
	bool bMouseInside;

	bool bDebugMode;

	int x0;

	int y0;

	int x1;

	int y1;

	int nWidth;

	int nHeight;
};

#endif // ifndef OTT_GUI_FRAME_HPP
