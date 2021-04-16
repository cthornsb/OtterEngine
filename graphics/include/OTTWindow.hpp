#ifndef OTT_WINDOW_HPP
#define OTT_WINDOW_HPP

#include <vector>
#include <queue>
#include <memory>
#include <string>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ColorRGB.hpp"
#include "OTTKeyboard.hpp"
#include "OTTMouse.hpp"
#include "OTTImageBuffer.hpp"
#include "Matrix.hpp"

class GPU;

class OTTJoypad;

namespace ott {
	enum class TextureModifier {
		NONE,
		VFLIP, ///< Flip texture about the x-axis
		HFLIP, ///< Flip texture about the y-axis
		ROTATE_CW, ///< Rotate texture 90 degrees clockwise
		ROTATE_CCW, ///< Rotate texture 90 degrees counter-clockwise
		ROTATE_180 ///< Rotate texture 180 degrees
	};
};

struct DestroyGLFWwindow {
	void operator()(GLFWwindow* ptr) {
		glfwDestroyWindow(ptr);
	}
};

class OTTWindow{
public:
	class VideoMode{
	public:
		int nWidth;
		
		int nHeight;
		
		int nColorDepth;
		
		int nRefreshRate;
		
		VideoMode() :
			nWidth(0),
			nHeight(0),
			nColorDepth(0),
			nRefreshRate(0)
		{
		}
		
		VideoMode(const GLFWvidmode* ptr) :
			nWidth(ptr->width),
			nHeight(ptr->height),
			nColorDepth(ptr->redBits + ptr->greenBits + ptr->blueBits),
			nRefreshRate(ptr->refreshRate)
		{
		}
	};

	/** Default constructor
	  */
	OTTWindow() = delete;
	
	/** Constructor taking the width and height of the window
	  */
	OTTWindow(const int &w, const int &h, const int& scale=1);
	
	/** Copy constructor
	  */
	OTTWindow(const OTTWindow&) = delete;
	
	/** Assignment operator
	  */
	OTTWindow& operator = (const OTTWindow&) = delete;

	/** Destructor
	  */
	virtual ~OTTWindow();
	
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
	
	/** Return true if the window is currently in full screen mode
	  */
	bool getFullScreenMode() const {
		return bFullScreenMode;
	}

	/** Return true if VSync is currently enabled
	  */
	bool getVSync() const {
		return bVSync;
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
	
	/** Get a pointer to the joypad handler.
	  * The user must call enableJoypad() to handle joypad callbacks.
	  */
	OTTJoypad* getJoypad() {
		return joypad;
	}
	
	/** Get the current system clipboard string
	  */
	std::string getClipboardString() const ;

	/** Get all available native monitor video modes
	  */
	int getAvailableVideoModes(std::vector<OTTWindow::VideoMode>& modes) const ;

	/** Get the current monitor video mode
	  */
	bool getCurrentVideoMode(OTTWindow::VideoMode& mode) const ;

	/** Get a pointer to the raw image buffer
	  */
	OTTImageBuffer* getBuffer() {
		return &buffer;
	}

	/** Get the OpenGL frame buffer texture context ID number
	  */
	GLuint getTextureBufferContext() const {
		return nTexture;
	}

	/** Print all available native monitor video modes
	  */
	void printVideoModes() const ;

	/** Set the size of graphical window
	  * Has no effect if window has not been initialized.
	  */
	void updateWindowSize(const int& w, const int& h, bool updatePhysicalSize = false);

	/** Set the size of graphical window
	  * Has no effect if window has not been initialized.
	  */
	void updateWindowSize(const int& scale);

	/** Enable or disable locking the window aspect ratio on window resize
	  */
	void lockWindowAspectRatio(bool state=true){
		bLockAspectRatio = state;
	}

	/** Set the current system clipboard string
	  */
	void setClipboardString(const std::string& str) const ;

	/** Set GLFW path / directory drop callback function.
	  * Function will be called for every system path dropped on the window.
	  */
	void setPathDropCallback(void (*callback)(const std::string&));

	/** Set GLFW window focus / unfocus callback function.
	  * Function will be called whenever the window gains or loses focus.
	  */
	void setWindowFocusCallback(void (*callback)(const bool&));

	/** Set the current draw color.
	  * After drawing is complete, ensure that the draw color is reset using resetDrawColor().
	  */
	void setDrawColor(ColorRGB *color);

	/** Set the current draw color.
	  * After drawing is complete, ensure that the draw color is reset using resetDrawColor().
	  */
	void setDrawColor(const ColorRGB &color);

	/** Reset the current draw color
	  */
	void resetDrawColor();

	/** Set this window as the current glfw window
	  * May be called from any thread.
	  */
	void setCurrent();

	/** Enable or disable full screen mode.
	  * The current monitor video mode will not be changed.
	  * The current state of the mouse cursor will be preserved if full screen mode is switched back later.
	  */
	void setFullScreenMode(bool state=true);

	/** Set the title string of the graphical window
	  */
	void setWindowTitle(const std::string& title);

	/** Toggle full screen mode.
	  * The current monitor video mode will not be changed.
	  * @return True if the window is now in full screen mode
	  */
	bool toggleFullScreenMode();
	
	/** Attempt to change the current monitor video mode.
	  * GLFW will attempt to change the video mode to the requested one, but the requested mode 
	  * may not be available. In that case, the closest matching mode will be selected.
	  * Attempting to change the video mode when not in full screen mode will have no effect.
	  */
	void changeVideoMode(const OTTWindow::VideoMode& mode);

	/** Clear the screen with a given color
	  */
	void clear(const ColorRGB &color=Colors::BLACK);

	/** Draw a single pixel at position (x, y) on the frame buffer.
	  * Use sparingly, as drawing directly to the frame buffer is costly.
	  */
	void drawPixel(const int &x, const int &y);

	/** Draw multiple pixels at positions (x1, y1) (x2, y2) ... (xN, yN) on the frame buffer.
	  * Use sparingly, as drawing directly to the frame buffer is costly.
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param N The number of elements in the arrays and the number of pixels to draw
	  */
	void drawPixels(const int *x, const int *y, const size_t &N);
	
	/** Draw a single line to the screen between pixel (x1, y1) and (x2, y2) on the frame buffer.
	  * Use sparingly, as drawing directly to the frame buffer is costly.
	  */
	void drawLine(const int &x0, const int &y0, const int &x1, const int &y1);

	/** Draw multiple lines directly to the frame buffer.
	  * Use sparingly, as drawing directly to the frame buffer is costly.
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param N The number of elements in the arrays. The total number of lines which will be drawn is N-1
	  */
	void drawLines(const int *x, const int *y, const size_t &N);

	/** Draw an OpenGL texture onto a portion of the frame buffer. The texture's upper left corner will appear 
	  * at pixel position (x0,y0), upper right at (x1,y1), bottom right at (x2,y2), and bottom left at (x3,y3).
	  * @param texture GL texture context
	  */
	void drawTexture(
		const unsigned int& texture,
		const int& x0, const int& y0,
		const int& x1, const int& y1,
		const int& x2, const int& y2,
		const int& x3, const int& y3
	);

	/** Draw an OpenGL texture onto a portion of the frame buffer. The texture will be confined to the rectangular
	  * bounding box whose upper left corner is at pixel position (x0,y0) and whose bottom right corner is at (x2,y2).
	  * @param texture GL texture context
	  */
	void drawTexture(const unsigned int& texture, const int& x0, const int& y0, const int& x2, const int& y2);

	/** Draw an OpenGL texture onto the frame buffer, covering the entire viewport
	  * @param texture GL texture context
	  */
	void drawTexture(const GLuint& texture);

	/** Draw an OpenGL texture onto the frame buffer, covering the entire viewport
	  * @param texture GL texture context
	  * @param modifier Texture display modifier enum
	  */
	void drawTexture(const GLuint& texture, const ott::TextureModifier& modifier);

	/** Draw an OpenGL texture onto the frame buffer, covering the entire viewport.
	  * The four corners of the texture may be placed in one of the four corners of the screen
	  * by specifying the vertex indicies i0, i1, i2, and i3 whose values must be 0-3 as shown below:
	  *  0 - Upper left corner of texture
	  *  1 - Upper right corner of texture
	  *  2 - Lower right corner of texture
	  *  3 - Lower left corner of texture
	  * These indicies allow the texture to be flipped and/or rotated as desired.
	  * @param texture GL texture context
	  */
	void drawTextureVertices(const unsigned int& texture, const int& i0, const int& i1, const int& i2, const int& i3);

	/** Transfer pixel data from an image buffer to a region on the texture buffer
	  * @param W Width of image buffer (in pixels)
	  * @param H Height of image buffer (in pixels)
	  * @param x0 X coordinate of top left corner
	  * @param y0 Y coordinate of top left corner
	  * @param data Pointer to RAM image buffer
	  */
	void drawBuffer(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, const OTTImageBuffer* data);

	/** Transfer pixel data from the RAM image buffer to the texture buffer, filling the entire texture
	  */
	void drawBuffer();

	/** Copy pixel data from an external RAM image buffer to this one.
	  * @param W Width of image buffer(in pixels)
	  * @param H Height of image buffer(in pixels)
	  * @param x0 X coordinate of top left corner on internal buffer where image data will be copied
	  * @param y0 Y coordinate of top left corner on internal buffer where image data will be copied
	  * @param data Pointer to external image buffer
	  */
	void buffWrite(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, OTTImageBuffer * data);

	/** Set a pixel on the RAM image buffer.
	  * When drawing on a pixel-by-pixel level, it is much more efficient to modify the image
	  * buffer in RAM and then periodically push the entire buffer to the GPU by calling
	  * drawBuffer() or renderBuffer() (e.g. 60 times per second).
	  */
	void buffWrite(const unsigned short& x, const unsigned short& y, const ColorRGB& color);

	/** Draw a horizontal line to RAM image buffer
	  */
	void buffWriteLine(const unsigned short& y, const ColorRGB& color);

	/** Display the current frame by swapping frame buffers
	  */
	void render();

	/** Transfer pixel data from the RAM image buffer to the texture buffer, draw the texture,
	  * and then display the texture by swapping frame buffers
	  */
	void renderBuffer();

	/** Return true if window is initialized and should not be closed.
	  * May be called from any thread.
	  */
	bool status();

	/** Initialize OpenGL and open the window
	  * Must be called from main thread.
	  */
	bool initialize(const std::string& name="OpenGL");

	/** Set pixel scaling factor
	  */
	void updatePixelZoom();

	/** Enable the RAM image buffer layer.
	  * The RAM image buffer requires the intermediate OpenGL texture buffer in order to be drawn to the
	  * frame buffer for display, so it will automatically be generated after the RAM buffer.
	  * @param bLinearFiltering If set, texture minimization and magnification are set to OpenGL linear filtering mode
	  * @return A pointer to the image buffer
	  */
	OTTImageBuffer* enableImageBuffer(bool bLinearFiltering = true);

	/** Enable the OpenGL texture buffer layer
	  * @param bLinearFiltering If set, texture minimization and magnification are set to OpenGL linear filtering mode
	  * @return The OpenGL texture context ID number
	  */
	GLuint enableTextureBuffer(bool bLinearFiltering = true);

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
	
	/** Enable gamepad button press event handling
	  */
	void enableGamepad();

	/** Disable gamepad button press event handling
	  */
	void disableGamepad();
	
	/** Enable vertical sync (VSync), if available.
	  * Availability varies by platform and hardware.
	  */
	void enableVSync();

	/** Disable vertical sync (VSync)
	  */
	void disableVSync();

	/** Enable OpenGL alpha blending for translucent textures
	  */
	void enableAlphaBlending();

	/** Disable alpha blending for opaque textures
	  */
	void disableAlphaBlending();

	/** Enable debug output mode.
	  * Will have no effect on GLFW, GLEW, and OpenGL if they have already been initialized.
	  */
	void enableDebugMode() {
		bDebugMode = true;
	}

	/** Disable debug output mode.
	  * Will have no effect on GLFW, GLEW, and OpenGL if they have already been initialized.
	  */
	void disableDebugMode() {
		bDebugMode = false;
	}

	/** Save current image buffer bitmap to disk
	  * @param fname Path to output image file
	  * @return True if image is saved successfully
	  */
	bool saveImageBufferToBitmap(const std::string& fname);

	/** Save current OpenGL frame buffer to disk
	  * @param fname Path to output image file
	  * @return True if image is saved successfully
	  */
	bool saveFrameBufferToBitmap(const std::string& fname);

	/** Handle glfw errors
	  */
	static void handleErrors(int error, const char* description);

	/** Handle window resizes
	  * @param W The new width of the window after the user has resized it
	  * @param H The new height of the window after the user has resized it
	  */
	static void handleReshapeScene(GLFWwindow* window, int W, int H);

	/** Handle window coming into and going out of focus
	  */
	static void handleWindowFocus(GLFWwindow* window, int focused);
	
	/** Handle GLFW window path / directory string drop event
	  */
	static void handlePathDrop(GLFWwindow* window, int count, const char** paths);

protected:
	std::unique_ptr<GLFWwindow, DestroyGLFWwindow> win; ///< Pointer to glfw graphics output window

	GLFWmonitor* monitor; ///< Pointer to primary display device

	GLuint nTexture; ///< OpenGL frame buffer texture context ID number

	int nNativeWidth; ///< Original width of the window (in pixels)
	
	int nNativeHeight; ///< Original height of the window (in pixels)
	
	float fNativeAspect; ///< Original aspect ratio of window

	int width; ///< Current width of window
	
	int height; ///< Current height of window
	
	float aspect; ///< Current aspect ratio of window
	
	int nOffsetX; ///< Horizontal buffer pixel offset
	
	int nOffsetY; ///< Vertical buffer pixel offset
	
	int nOldWidth; ///< Width of window before switching to full screen mode
	
	int nOldHeight; ///< Height of window before switching to full screen mode
	
	int nOldPosX; ///< Horizontal position of window before switching to full screen mode
	
	int nOldPosY; ///< Vertical position of window before switching to full screen mode
	
	bool init; ///< Flag indicating that the window has been initialized

	bool bFirstInit; ///< Set if this is the first initialization

	bool bLockAspectRatio; ///< If set, original aspect ratio will be preserved on window resize

	bool bFullScreenMode; ///< Set if window is in full screen mode

	bool bVSync; ///< Set if VSync is enabled

	bool bDebugMode; ///< Set if debug output mode is enabled

	Matrix4 mOrthoProjection; ///< Orthographic projection matrix (for 2d graphics)

	OTTKeyboard keys; ///< GLFW keyboard callback wrapper
	
	OTTMouse mouse; ///< GLFW mouse callback wrapper
	
	OTTJoypad* joypad; ///< GLFW joypad callback wrapper
	
	OTTImageBuffer buffer; ///< RAM frame buffer
	
	MouseStates previousMouseState; ///< State of the mouse cursor before full-screen mode switch
	
	void (*userPathDropCallback)(const std::string&);

	void (*userWindowFocusCallback)(const bool&);
	
	/** Update viewport and projection matrix after resizing window
	  * This method should be called for setting up a viewport and projection matrix for 2d displays.
	  */
	void reshape();
	
	/** Call the user path-drop callback function for each of the dropped system paths
	  */
	void dropSystemPaths(const std::vector<std::string>& path);
	
	/** Call the user window focus callback function whenever the window gains or loses focus
	  */
	void setWindowFocus(const bool& focused);

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

