#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <vector>
#include <queue>
#include <string>

#include "ColorRGB.hpp"
#include "Shader.hpp"
#include "Vector.hpp"

class object;
class Shader;

/// <summary>
/// Simple mouse motion and button press wrapper for interfacing with GLUT
/// </summary>
class MouseState {
public:
	/** Default constructor
	  **/
	MouseState();

	/** Lock the cursor to the center of the screen
	  **/
	void setLockPointer(bool state = true) { lockPointer = state; }

	/** Set the horizontal position of the cursor (in pixels)
	  **/
	void setX(const int& x) { posX = x; }

	/** Set the vertical position of the cursor (in pixels)
	  **/
	void setY(const int& y) { posY = y; }

	/** Set the position of the cursor (in pixels)
	  **/
	void setPosition(const int& x, const int& y);

	/** Get the current horizontal position of the cursor (in pixels)
	  **/
	int getX() const { return posX; }

	/** Get the current vertical position of the cursor (in pixels)
	  **/
	int getY() const { return posY; }

	/** Get the change in horizontal position of the cursor since the last call to setPosition()
	  * @note Does not reset the delta position values
	  **/
	int getDeltaX() const { return dX; }

	/** Get the change in vertical position of the cursor since the last call to setPosition()
	  * @note Does not reset the delta position values
	  **/
	int getDeltaY() const { return dY; }

	/** Return true if no mouse button is pressed, and return false otherwise
	  **/
	bool empty() const { return (count == 0); }

	/** Return true if the mouse has moved since the last call to setPosition(), and return false otherwise
	  **/
	bool delta();

	/** Get the change in the horizontal and vertical position of the mouse and reset the delta values 	
	  * @return True if the mouse has moved since the last call to setPosition(), and return false otherwise
	  **/
	bool delta(int& dx, int& dy);

	/** Check if a mouse button is pressed but do not reset its state
	  * @note This method should be used for mouse buttons which are held down
	  **/
	bool check(const unsigned char& button) const { return states[button]; }

	/** Check if a mouse button is pressed and reset its state
	  * @note This method should be used for mouse button presses, where holding the button down is ignored
	  **/
	bool poll(const int& button);

	/** Set a mouse button as pressed
	  * @note Typically this method should be called by the external mouse callback
	  **/
	void down(const int& button);

	/** Set a mouse button as released
	  * @note Typically this method should be called by the external mouse callback
	  **/
	void up(const int& button);

	/** Reset the state of all mouse buttons
	  **/
	void reset();

private:
	unsigned short count; ///< Number of standard keyboard keys which are currently pressed

	int posX; ///< Current mouse position X on the screen
	int posY; ///< Current mouse position Y on the screen

	int dX; ///< Delta X position of mouse
	int dY; ///< Delta Y position of mouse

	bool lockPointer; ///< "Locks" the pointer to the middle of the window

	bool states[3]; ///< Mouse button states for left, middle, and right buttons (true indicates button is down) 
};

/// <summary>
/// Simple keyboard key press wrapper for interfacing with GLUT
/// </summary>
class KeyStates{
public:
	/** Default constructor
	  **/
	KeyStates();
	
	/** Enable character stream mode
	  * Characters may be retrieved from the character buffer by calling get(). 
	  * This mode should be used for text entry e.g. a word processor program.
	  **/
	void enableStreamMode();
	
	/** Disable character stream mode and return to default keypress mode
	  **/
	void disableStreamMode();
	
	/** Return true if no key is pressed, and return false otherwise
	  **/
	bool empty() const { return (count == 0); }

	/** Check if a key is pressed but do not reset its state
	  * @note This method should be used for keys which are held down
	  **/
	bool check(const unsigned char &key) const { return states[key]; }

	/** Check if a mouse button is pressed and reset its state
	  * @note This method should be used for key presses, where holding the key down is ignored
	  **/
	bool poll(const unsigned char &key);

	/** Set a key as pressed
	  * @note Typically this method should be called by the external keyboard callback
	  **/
	void keyDown(const unsigned char &key);

	/** Set a key as released
	  * @note Typically this method should be called by the external keyboard callback
	  **/
	void keyUp(const unsigned char &key);
	
	/** Get a character from the character stream buffer
	  * @return True if there was at least one character in the buffer, and return false if the buffer is empty
	  **/
	bool get(char& key);

	/** Reset the state of all keyboard keys and empty the character buffer
	  **/
	void reset();

private:
	unsigned short count; ///< Number of standard keyboard keys which are currently pressed

	bool streamMode; ///< Flag to set keyboard to behave as stream buffer

	std::queue<char> buffer; ///< Character input buffer

	bool states[256]; ///< States of keyboard keys (true indicates key is down) 
};

/// <summary>
/// Wrapper class for interfacing with GLUT and OpenGL, and for managing graphical windows
/// </summary>
class Window{
public:
	/** Default constructor
	  */
	Window() : 
		W(0), 
		H(0), 
		A(1),
		width(0),
		height(0),
		aspect(1),
		nMult(1), 
		winID(0), 
		init(false),
		keys(),
		mouse()
	{ 
	}
	
	/** Constructor taking the width and height of the window
	  */
	Window(const int &w, const int &h, const int& scale=1) : 
		W(w), 
		H(h),
		A(float(w)/h),
		width(w),
		height(h),
		aspect(float(w)/h),
		nMult(scale), 
		winID(0),
		init(false),
		keys(),
		mouse()
	{
	}

	/** Destructor
	  */
	~Window();
	
	/** Destroy the graphical window
	  **/
	void close();

	/** Process GLUT callback events
	  * This method should be called once per iteration of the main loop.
	  **/
	bool processEvents();

	/** Get the current width of the window (in pixels)
	  */
	int getCurrentWidth() const { return width; }
	
	/** Get the current height of the window (in pixels)
	  */
	int getCurrentHeight() const { return height; }

	/** Get the initial width of the window (in pixels)
	  */
	int getWidth() const { return W; }
	
	/** Get the initial height of the window (in pixels)
	  */
	int getHeight() const { return H; }

	/** Get screen pixel multiplier
	  */
	int getScale() const { return nMult; }

	/** Get the current aspect ratio of the window (W/H)
	  */
	float getCurrentAspectRatio() const { return aspect; }

	/** Get the initial aspect ratio of the window (W/H)
	  */
	float getAspectRatio() const { return A; }

	/** Get the GLUT window ID number
	  */
	int getWindowID() const { return winID; }

	/** Get a pointer to the keyboard handler
	  * The user must call setupKeyboardHandler() to handle keyboard callbacks
	  */
	KeyStates* getKeypress(){ return &keys; }

	/** Get a pointer to the mouse handler
	  * The user must call setupMouseHandler() to handle mouse callbacks
	  */
	MouseState* getMouse() { return &mouse; }

	/** Get a pointer to a loaded shader
	  */
	Shader* getShader(const ShaderType& type);

	/** Set the width of the window (in pixels)
	  */
	void setWidth(const int &w){ width = w; }
	
	/** Set the height of the window (in pixels)
	  */
	void setHeight(const int &h){ height = h; }

	/** Set the integer pixel scaling multiplier (default = 1)
	  */
	void setScalingFactor(const int &scale);

	/** Set the current draw color
	  */
	static void setDrawColor(ColorRGB *color, const float &alpha=1);

	/** Set the current draw color
	  */
	static void setDrawColor(const ColorRGB &color, const float &alpha=1);

	/** Set this window as the current GLUT window
	  */
	void setCurrent();

	/** Clear the screen with a given RGB color
	  */
	static void clear(const ColorRGB &color=Colors::BLACK);

	/** Draw a pixel at position (x, y) on the screen
	  */
	static void drawPixel(const int &x, const int &y);

	/** Draw multiple pixels at positions (x1, y1) (x2, y2) ... (xN, yN) on the screen
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param N The number of elements in the arrays and the number of pixels to draw
	  */
	static void drawPixel(const int *x, const int *y, const size_t &N);

	/** Draw a pixel at 3d position (x, y, z) on the screen
	  */
	static void drawPixel(const float& x, const float& y, const float& z);

	/** Draw a line between points (x1, y1) and (x2, y2) on the screen
	  */
	static void drawLine(const int &x1, const int &y1, const int &x2, const int &y2);

	/** Draw multiple lines on the screen
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param N The number of elements in the arrays. Since it is assumed that the number of elements 
		       in the arrays is equal to @a N, the total number of lines which will be drawn is equal to N-1
	  */
	static void drawLine(const int *x, const int *y, const size_t &N);

	/** Draw a line between 3d points (x1, y1, z1) and (x2, y2, z2) on the screen
	  */
	static void drawLine(
		const float& x1, const float& y1, const float& z1,
		const float& x2, const float& y2, const float& z2
	);

	/** Draw a rectangle on the screen
	  * @param x1 X coordinate of the upper left corner
	  * @param y1 Y coordinate of the upper left corner
	  * @param x2 X coordinate of the bottom right corner
	  * @param y2 Y coordinate of the bottom right corner
	  */
	static void drawRectangle(const int &x1, const int &y1, const int &x2, const int &y2);

	/** Draw OpenGL texture on the screen within the specified bounds
	  * @param texture GL texture context
	  * @param x1 X coordinate of the upper left corner
	  * @param y1 Y coordinate of the upper left corner
	  * @param x2 X coordinate of the bottom right corner
	  * @param y2 Y coordinate of the bottom right corner
	 **/
	static void drawTexture(const unsigned int& texture, const int& x1, const int& y1, const int& x2, const int& y2);

	/** Draw an array of raw 3d vertices
	  * @param vertices Array of raw 3d position components formatted as {x0, y0, z0, ... , xN-1, yN-1, zN-1}
	  * @param indicies Vector of vertex indicies representing the triangles which will be drawn
	  */
	static void drawVertexArray(const float* vertices, const std::vector<unsigned short>& indices);

	/** Draw a 3d object on the screen
	  * @param obj Pointer to 3d object which will be drawn
	  */
	void drawObject(const object* obj);

	/** Draw a static 3d object on the screen
	  * @param obj Pointer to 3d object which will be drawn
	  * @param offset Position offset of object (camera position)
	  */
	void drawStaticObject(const object* obj, const Vector3& offset);

	/** Render the current frame
	  */
	static void render();

	/** Return true if the window is open and return false otherwise
	  */
	bool status();

	/** Initialize OpenGL and open a window with a given name
	  */
	void initialize(const std::string& name="OpenGL");

	/** Enable keyboard character stream mode
	  * This mode should be used for text entry e.g. a word processor program.
	  **/
	void setKeyboardStreamMode();

	/** Disable keyboard character stream mode and return to default keypress mode
	  **/
	void setKeyboardToggleMode();

	/** Setup GLUT keyboard callback to capture keyboard key presses in this window
	  **/
	void setupKeyboardHandler();

	/** Setup GLUT mouse callback to capture mouse movement and button presses in this window
	**/
	void setupMouseHandler();

	/** Enable OpenGL alpha blending for translucent textures
	  **/
	void enableAlphaBlending();

	/** Enable OpenGL Z-depth test and setup viewport for perspective 3d mode
	  * This method must be called for rendering 3d objects as the default rendering mode is 2d
	  **/
	void enableZDepth();

	/** Disable OpenGL Z-depth test and setup viewport for 2d drawing mode (default)
	  **/
	void disableZDepth();

	/** Enable OpenGL backface culling
	  * OpenGL perspective mode must be enabled, otherwise this will have no effect
	  **/
	void enableCulling();

	/** Disable OpenGL backface culling
	  */
	void disableCulling();

	/** Switch to wireframe mode
	  * OpenGL perspective mode must be enabled, otherwise this will have no effect
	  */
	void enableWireframeMode();

	/** Disable wireframe mode (default)
	  */
	void disableWireframeMode();

	/** Enable the use of an OpenGL shader program
	  */
	void enableShader(const ShaderType& type);

	/** Enable the use of an OpenGL shader program
	  */
	void enableShader(const Shader* shdr);

	/** Disable the use of an OpenGL shader program
	  */
	void disableShader();

	/** Reset the OpenGL modelview matrix to identity
	  */
	void resetModelviewMatrix();

	void translateModelviewMatrix(const Vector3& pos);

	void rotateModelviewMatrix(const float& x, const float& y, const float& z);

	/** QT OpenGL callback, currently not used
	  **/
	virtual void paintGL();
	
	/** QT OpenGL callback, currently not used
	  **/
	virtual void initializeGL();

	/** QT OpenGL callback, currently not used
	  **/
	virtual void resizeGL(int width, int height);

private:
	int W; ///< Original width of the window (in pixels)
	int H; ///< Original height of the window (in pixels)
	float A; ///< Original aspect ratio of window

	int width; ///< Current width of window (in pixels)
	int height; ///< Current height of window (in pixels)
	float aspect; ///< Current aspect ratio of window
	
	int nMult; ///< Integer multiplier for window scaling
	int winID; ///< GLUT window identifier

	bool init; ///< Flag indicating that the window has been initialized

	KeyStates keys; ///< GLUT keyboard event callback wrapper

	MouseState mouse; ///< GLUT mouse event callback wrapper

	std::unique_ptr<DefaultShaders::ShaderList> shaders;
};
#endif
