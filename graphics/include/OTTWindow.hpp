#ifndef OTT_WINDOW_HPP
#define OTT_WINDOW_HPP

#include <vector>
#include <queue>
#include <memory>
#include <map>

#include <GLFW/glfw3.h>

#include "ColorRGB.hpp"
#include "OTTKeyboard.hpp"
#include "OTTMouse.hpp"
#include "OTTImageBuffer.hpp"

class GPU;

struct DestroyGLFWwindow {
	void operator()(GLFWwindow* ptr) {
		glfwDestroyWindow(ptr);
	}
};

class OTTWindow{
public:
	/** Default constructor
	  */
	OTTWindow() = delete;
	
	/** Constructor taking the width and height of the window
	  */
	OTTWindow(const int &w, const int &h, const int& scale=1) : 
		win(),
		nNativeWidth(w), 
		nNativeHeight(h),
		fNativeAspect(float(w)/h),
		width(nNativeWidth * scale),
		height(nNativeHeight * scale),
		aspect(fNativeAspect),
		init(false),
		bFirstInit(true),
		keys(),
		mouse(),
		buffer()
	{
	}

	/** Copy constructor
	  */
	OTTWindow(const OTTWindow&) = delete;
	
	/** Assignment operator
	  */
	OTTWindow& operator = (const OTTWindow&) = delete;

	/** Destructor
	  */
	~OTTWindow();
	
	/** Set window to be closed
	  */
	void close();

	/** Poll OpenGL events
	  * Must be called from main thread.
	  */
	bool processEvents();

	/** Get the width of the window (in pixels)
	  */
	int getNativeWidth() const {
		return nNativeWidth; 
	}
	
	/** Get the height of the window (in pixels)
	  */
	int getNativetHeight() const {
		return nNativeHeight;
	}

	/** Get the aspect ratio of the window (W/H)
	  */
	float getNativeAspectRatio() const {
		return fNativeAspect;
	}

	/** Get the current width of the window (in pixels)
	  */
	int getWidth() const {
		return width;
	}
	
	/** Get the current height of the window (in pixels)
	  */
	int getHeight() const {
		return height;
	}

	/** Get the aspect ratio of the window (W/H)
	  */
	float getAspectRatio() const {
		return aspect;
	}

	/** Get a pointer to the last user keypress event
	  * The user must call enableKeyboard() to handle keyboard callbacks
	  */
	OTTKeyboard* getKeypress(){
		return &keys;
	}
	
	/** Get a pointer to the mouse handler
	  * The user must call enableMouse() to handle mouse callbacks
	  */
	OTTMouse* getMouse() { 
		return &mouse; 
	}

	/** Set the size of graphical window
	  * Has no effect if window has not been initialized.
	  */
	void updateWindowSize(const int& w, const int& h);

	/** Set the size of graphical window
	  * Has no effect if window has not been initialized.
	  */
	void updateWindowSize(const int& scale);

	/** Set the current draw color
	  */
	static void setDrawColor(ColorRGB *color, const float &alpha=1);

	/** Set the current draw color
	  */
	static void setDrawColor(const ColorRGB &color, const float &alpha=1);

	/** Set this window as the current glfw window
	  * May be called from any thread.
	  */
	void setCurrent();

	/** Clear the screen with a given color
	  */
	static void clear(const ColorRGB &color=Colors::BLACK);

	/** Draw a single pixel at position (x, y)
	  */
	static void drawPixel(const int &x, const int &y);

	/** Draw multiple pixels at positions (x1, y1) (x2, y2) ... (xN, yN)
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param N The number of elements in the arrays and the number of pixels to draw
	  */
	static void drawPixel(const int *x, const int *y, const size_t &N);
	
	/** Draw a single line to the screen between points (x1, y1) and (x2, y2)
	  */
	static void drawLine(const int &x1, const int &y1, const int &x2, const int &y2);

	/** Draw multiple lines to the screen
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param N The number of elements in the arrays. Since it is assumed that the number of elements 
		       in the arrays is equal to @a N, the total number of lines which will be drawn is equal to N-1
	  */
	static void drawLine(const int *x, const int *y, const size_t &N);

	/** Draw multiple lines to the screen
	  * @param x1 X coordinate of the upper left corner
	  * @param y1 Y coordinate of the upper left corner
	  * @param x2 X coordinate of the bottom right corner
	  * @param y2 Y coordinate of the bottom right corner
	  */
	static void drawRectangle(const int &x1, const int &y1, const int &x2, const int &y2);

	/** Write pixel data from an array to the GPU frame buffer
	  * @param width Width of bitmap (in pixels)
	  * @param height Height of bitmap (in pixels)
	  * @param x0 X coordinate of bottom left corner
	  * @param y0 Y coordinate of bottom left corner
	  * @param data Array of bitmap pixel data
	  */
	static void drawBitmap(const unsigned int& width, const unsigned int& height, const int& x0, const int& y0, const unsigned char* data);

	/** Write pixel data from an image buffer to the GPU frame buffer
	  * @param width Width of image buffer (in pixels)
	  * @param height Height of image buffer (in pixels)
	  * @param x0 X coordinate of bottom left corner
	  * @param y0 Y coordinate of bottom left corner
	  * @param data Pointer to conventional-memory image buffer
	  */
	static void drawPixels(const unsigned int& width, const unsigned int& height, const int& x0, const int& y0, const OTTImageBuffer* data);

	/** Write to CPU image buffer
	  * When drawing on a pixel-by-pixel level, it is much more efficient to modify the image
	  * buffer in CPU memory and then periodically push the entire buffer to the GPU by calling
	  * drawBuffer() (e.g. 60 times per second).
	  */
	void buffWrite(const unsigned short& x, const unsigned short& y, const ColorRGB& color);

	/** Draw a horizontal line to CPU frame buffer
	  */
	void buffWriteLine(const unsigned short& y, const ColorRGB& color);

	/** Render the current frame
	  */
	void render();

	/** Draw the image buffer but do not render the frame
	  * Useful if you wish to do additional processing on top of the image buffer.
	  */
	void drawBuffer();

	/** Draw the image buffer and render the frame
	  */
	void renderBuffer();

	/** Return true if window is uninitialized or if it should be closed
	  * May be called from any thread.
	  */
	bool status();

	/** Initialize OpenGL and open the window
	  * Must be called from main thread.
	  */
	void initialize(const std::string& name="OpenGL");

	/** Set pixel scaling factor
	  */
	void updatePixelZoom();

	/** Enable keyboard keypress event handling
	  */
	void enableKeyboard();
	
	/** Disable keyboard keypress event handling
	  */
	void disableKeyboard();
	
	/** Set keyboard handler to character stream buffer mode (e.g. a word-processor type program)
	  */
	void setKeyboardStreamMode();

	/** Set keyboard handler to instantaneous keypress mode (default)
	  */
	void setKeyboardToggleMode();

	/** Enable mouse movement and button press event handling
	  */
	void enableMouse();

	/** Disable mouse movement and button press event handling
	  */
	void disableMouse();
	
	/** Handle glfw errors
	  */
	static void handleErrors(int error, const char* description);

	/** Handle window resizes
	  * @param width The new width of the window after the user has resized it
	  * @param height The new height of the window after the user has resized it
	  */
	static void handleReshapeScene(GLFWwindow* window, int width, int height);

	/** Handle window coming into and going out of focus
	  */
	static void handleWindowFocus(GLFWwindow* window, int focused);
	
protected:
	std::unique_ptr<GLFWwindow, DestroyGLFWwindow> win; ///< Pointer to glfw graphics output window

	int nNativeWidth; ///< Original width of the window (in pixels)
	
	int nNativeHeight; ///< Original height of the window (in pixels)
	
	float fNativeAspect; ///< Original aspect ratio of window

	int width; ///< Current width of window
	
	int height; ///< Current height of window
	
	float aspect; ///< Current aspect ratio of window
	
	bool init; ///< Flag indicating that the window has been initialized

	bool bFirstInit; ///< Set if this is the first initialization

	OTTKeyboard keys; ///< The last key which was pressed by the user
	
	OTTMouse mouse; ///< GLUT mouse event callback wrapper
	
	OTTImageBuffer buffer; ///< CPU-side frame buffer
	
	/** Update viewport and projection matrix after resizing window
	  * This method should be called for setting up a viewport and projection matrix for 2d displays.
	  */
	void reshape();
	
	/** Called whenever the user updates the size of the window
	  */
	virtual void onUserReshape(){
		reshape();
	}
	
	/** Additional window initialization called from initialize()
	  */
	virtual void onUserInitialize() { }
};

class OTTActiveWindows{
public:
	/** Copy constructor
	  */
	OTTActiveWindows(const OTTActiveWindows&) = delete;

	/** Assignment operator
	  */
	OTTActiveWindows& operator = (const OTTActiveWindows&) = delete;
	
	/** Get reference to the singleton
	  */
	static OTTActiveWindows& get();
	
	/** Add a GLFW window to the map
	  */
	void add(GLFWwindow* glfw, OTTWindow* win);
	
	/** Find a pointer to an opengl window object
	  */
	OTTWindow* find(GLFWwindow* glfw);
	
private:
	std::map<GLFWwindow*, OTTWindow*> listOfWindows; ///< Map of GLFW windows

	/** Default constructor (private)
	  */
	OTTActiveWindows() :
		listOfWindows()
	{ 
	}
};

#endif

