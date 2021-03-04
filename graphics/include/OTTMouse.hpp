#ifndef OTT_MOUSE_HPP
#define OTT_MOUSE_HPP

class GLFWwindow;
class GLFWcursor;

/// <summary>
/// Simple mouse motion and button press wrapper for interfacing with glfw
/// </summary>
class OTTMouse {
public:
	/** Default constructor
	  **/
	OTTMouse();

	/** Set pointer to parent window
	  */
	void setParentWindow(GLFWwindow* ptr){
		parent = ptr;
	}

	/** Lock the cursor to the center of the screen
	  **/
	void setLockPointer(bool state = true) {
		bLockPointer = state;
	}

	/** Set the horizontal position of the cursor (in pixels)
	  **/
	void setX(const int& x) {
		dPosX = x;
	}

	/** Set the vertical position of the cursor (in pixels)
	  **/
	void setY(const int& y) {
		dPosY = y;
	}

	/** Set the position of the cursor (in pixels)
	  **/
	void setPosition(const int& x, const int& y);

	/** Get the current horizontal position of the cursor (in pixels)
	  **/
	int getX() const {
		return dPosX;
	}

	/** Get the current vertical position of the cursor (in pixels)
	  **/
	int getY() const {
		return dPosY;
	}

	/** Get the change in horizontal position of the cursor since the last call to setPosition()
	  * @note Does not reset the delta position values
	  **/
	int getDeltaX() const {
		return dDeltaX;
	}

	/** Get the change in vertical position of the cursor since the last call to setPosition()
	  * @note Does not reset the delta position values
	  **/
	int getDeltaY() const {
		return dDeltaY;
	}

	/** Return true if no mouse button is pressed, and return false otherwise
	  **/
	bool empty() const {
		return (nCount == 0);
	}

	/** Enable mouse callbacks
	  * Required in order to handle mouse input.
	  */
	void enable();
	
	/** Disable mouse callbacks
	  */
	void disable();

	/** Cursor functions as normal and its motion is not limited
	  */
	void setNormalCursor();

	/** Cursor is hidden and locked to the window
	  */
	void setDisableCursor();

	/** Cursor is hidden, but functions as normal
	  */
	void setHideCursor();

	/** Set raw cursor input mode, if available on this sytem
	  * If cursor is not currently disabled, it will be disabled.
	  * @return True if raw input mode was enabled
	  */
	bool setRawCursor();

	/** Set a custom mouse cursor image
	  * @param pixels Array of pixel color data containing four bytes per pixel (RGBA)
	  * @param w Width of new cursor image (in pixels)
	  * @param h Height of new cursor image (in pixels)
	  * @param x0 Horizontal coordinate of active point of cursor (with x0 = y0 = 0 at the upper left corner)
	  * @param y0 Vertical coordinate of active point of cursor (with x0 = y0 = 0 at the upper left corner)
	  * @return A pointer to the new GLFWcursor
	  */
	GLFWcursor* setCustomCursor(unsigned char* pixels, const int& w, const int& h, const int& x0=0, const int& y0=0);

	/** Delete custom mouse cursor image and revert to default cursor
	  */
	void removeCustomCursor();

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
	bool check(const unsigned char& button) const {
		return bStates[button];
	}

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

	static void handleMouse(GLFWwindow* window, int button, int action, int mods);

	static void handleMouseMove(GLFWwindow* window, double x, double y);

	static void handleMouseEnter(GLFWwindow* window, int entered);

	static void handleMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

private:
	unsigned short nCount; ///< Number of standard keyboard keys which are currently pressed

	double dPosX; ///< Current mouse position X on the screen
	
	double dPosY; ///< Current mouse position Y on the screen

	double dDeltaX; ///< Delta X position of mouse
	
	double dDeltaY; ///< Delta Y position of mouse

	bool bLockPointer; ///< "Locks" the pointer to the middle of the window

	bool bStates[3]; ///< Mouse button states for left, middle, and right buttons (true indicates button is down) 

	GLFWcursor* cursor; ///< Pointer to custom mouse cursor (if in use)
	
	GLFWwindow* parent; ///< Pointer to parent window
	
	/** Setup glfw mouse callback functions
	  */
	void setupCallbacks(bool bEnable=true);
};

#endif
