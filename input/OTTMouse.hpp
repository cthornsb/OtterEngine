#ifndef INPUT_OTT_MOUSE_HPP
#define INPUT_OTT_MOUSE_HPP

#include <cstdint>

struct GLFWwindow;
struct GLFWcursor;

namespace ott {

/// <summary>
/// GLFW mouse cursor states
/// </summary>
enum class MouseStates{
	Normal,   ///< Cursor visible and behaves normally
	Disabled, ///< Cursor is hidden and is locked inside the window
	Hidden,   ///< Cursor is hidden but is not restricted from leaving the window
	Raw       ///< 
};

/// <summary>
/// Simple mouse motion and button press wrapper for interfacing with glfw
/// </summary>
class Mouse {
public:
	/** Copy constructor
	  */
	Mouse(const Mouse&) = delete;

	/** Assignment operator
	  */
	Mouse& operator = (const Mouse&) = delete;

	/** Get a reference to the singleton joypad object
	  */
	static Mouse& Instance();

	/** Set pointer to parent window
	  */
	void SetParentWindow(GLFWwindow* ptr){
		parent = ptr;
	}

	/** Set the horizontal position of the cursor (in pixels)
	  **/
	void SetX(const double& x) {
		dPosX = x;
	}

	/** Set the vertical position of the cursor (in pixels)
	  **/
	void SetY(const double& y) {
		dPosY = y;
	}

	/** Set the position of the cursor (in pixels)
	  **/
	void SetPosition(const double& x, const double& y);

	/** Get the current horizontal position of the cursor (in pixels)
	  **/
	double X() const {
		return dPosX;
	}

	/** Get the current vertical position of the cursor (in pixels)
	  **/
	double Y() const {
		return dPosY;
	}

	/** Get the change in horizontal position of the cursor since the last call to setPosition()
	  * @note Does not reset the delta position values
	  **/
	double DeltaX() const {
		return dDeltaX;
	}

	/** Get the change in vertical position of the cursor since the last call to setPosition()
	  * @note Does not reset the delta position values
	  **/
	double DeltaY() const {
		return dDeltaY;
	}

	/** Get the current mouse state
	  */
	MouseStates CursorState() const {
		return state;
	}

	/** Return true if no mouse button is pressed, and return false otherwise
	  **/
	bool Empty() const {
		return (nCount == 0);
	}

	/** Enable mouse callbacks
	  * Required in order to handle mouse input.
	  */
	void Enable();
	
	/** Disable mouse callbacks
	  */
	void Disable();

	/** Cursor functions as normal and its motion is not limited
	  */
	void NormalCursor();

	/** Cursor is hidden and locked to the window
	  */
	void DisableCursor();

	/** Cursor is hidden, but functions as normal
	  */
	void HideCursor();

	/** Set raw cursor input mode, if available on this sytem
	  * If cursor is not currently disabled, it will be disabled.
	  * @return True if raw input mode was enabled
	  */
	bool RawCursor();

	/** Set new mouse cursor state
	  * @return True if new cursor state set successfully
	  */
	bool SetCursorState(const MouseStates& newState);

	/** Set a custom mouse cursor image
	  * @param pixels Array of pixel color data containing four bytes per pixel (RGBA)
	  * @param w Width of new cursor image (in pixels)
	  * @param h Height of new cursor image (in pixels)
	  * @param x0 Horizontal coordinate of active point of cursor (with x0 = y0 = 0 at the upper left corner)
	  * @param y0 Vertical coordinate of active point of cursor (with x0 = y0 = 0 at the upper left corner)
	  * @return A pointer to the new GLFWcursor
	  */
	GLFWcursor* SetCustomCursor(uint8_t* pixels, const int32_t& w, const int32_t& h, const int32_t& x0=0, const int32_t& y0=0);

	/** Delete custom mouse cursor image and revert to default cursor
	  */
	void RemoveCustomCursor();

	/** Return true if the mouse has moved since the last call to setPosition(), and return false otherwise
	  **/
	bool Delta();

	/** Get the change in the horizontal and vertical position of the mouse and reset the delta values 	
	  * @return True if the mouse has moved since the last call to setPosition(), and return false otherwise
	  **/
	bool Delta(double& dx, double& dy);

	/** Check if a mouse button is currently being held
	  **/
	bool Check(const uint8_t& button) const {
		return bStates[button];
	}

	/** Check if a mouse button was pressed
	  * @note This method should be used for mouse button presses, where holding the button down is ignored
	  **/
	bool Poll(const int32_t& button);

	/** Check if a mouse button was pressed (performs the same function as Mouse::poll)
	  * @note This method should be used for mouse button presses, where holding the button down is ignored
	  **/
	bool Pressed(const int32_t& button);

	/** Check if a mouse button was released
	  **/
	bool Released(const int32_t& button);

	/** Set a mouse button as pressed
	  * @note Typically this method should be called by the external mouse callback
	  **/
	void Down(const int32_t& button);

	/** Set a mouse button as released
	  * @note Typically this method should be called by the external mouse callback
	  **/
	void Up(const int32_t& button);

	/** Reset the state of all mouse buttons
	  **/
	void Reset();

	static void HandleMouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods);

	static void HandleMouseMove(GLFWwindow* window, double x, double y);

	static void HandleMouseEnter(GLFWwindow* window, int32_t entered);

	static void HandleMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

private:
	MouseStates state; ///< State of the mouse cursor before a mode change

	uint16_t nCount; ///< Number of standard keyboard keys which are currently pressed

	double dPosX; ///< Current mouse position X on the screen
	
	double dPosY; ///< Current mouse position Y on the screen

	double dDeltaX; ///< Delta X position of mouse
	
	double dDeltaY; ///< Delta Y position of mouse

	bool bStates[3]; ///< Flags which are set when left, right, and middle mouse buttons are currently held	

	bool bPressed[3]; ///< Flags set when the left, right, and middle mouse buttons are pressed

	bool bReleased[3]; ///< Flags set when the left, right, and middle mouse buttons are released

	GLFWcursor* cursor; ///< Pointer to custom mouse cursor (if in use)
	
	GLFWwindow* parent; ///< Pointer to parent window
	
	/** Default constructor
	  */
	Mouse();

	/** Setup glfw mouse callback functions
	  */
	void SetupCallbacks(bool bEnable=true);
};

} /* namespace ott */

#endif // #ifndef INPUT_OTT_MOUSE_HPP
