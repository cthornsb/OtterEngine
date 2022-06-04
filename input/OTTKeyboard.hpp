#ifndef INPUT_OTT_KEYBOARD_HPP
#define INPUT_OTT_KEYBOARD_HPP

#include <cstdint>
#include <queue>

struct GLFWwindow;

namespace ott {

class Keyboard {
public:
	/** Copy constructor
	  */
	Keyboard(const Keyboard&) = delete;

	/** Assignment operator
	  */
	Keyboard& operator = (const Keyboard&) = delete;

	/** Get a reference to the singleton joypad object
	  */
	static Keyboard& Instance();

	/** Set pointer to parent window
	  */
	void SetParentWindow(GLFWwindow* ptr){
		parent = ptr;
	}

	/** Enable mouse callbacks
	  * Required in order to handle mouse input.
	  */
	void Enable();
	
	/** Disable mouse callbacks
	  */
	void Disable();
	
	/** Enable text stream mode
	  */
	void EnableStreamMode();
	
	/** Disable text stream mode and return to keypress mode
	  */
	void DisableStreamMode();
	
	/** Return true if no keys are currently pressed
	  */
	bool Empty() const { 
		return (nCount == 0); 
	}
	
	/** Check the current state of a key without modifying it
	  */
	bool Check(const uint8_t &key) const { 
		return states[key]; 
	}

	/** Return true if left or right shift is currently pressed
	  */
	bool Shift() const {
		return (bLeftShift || bRightShift);
	}

	/** Return true if left shift is currently pressed
	  */	
	bool LeftShift() const {
		return bLeftShift;
	}

	/** Return true if right shift is currently pressed
	  */	
	bool RightShift() const {
		return bLeftShift;
	}

	/** Return true if left or right ctrl is currently pressed
	  */
	bool Ctrl() const {
		return (bLeftCtrl || bRightCtrl);
	}

	/** Return true if left ctrl is currently pressed
	  */	
	bool LeftCtrl() const {
		return bLeftShift;
	}

	/** Return true if right ctrl is currently pressed
	  */	
	bool RightCtrl() const {
		return bLeftShift;
	}

	/** Return true if left or right alt is currently pressed
	  */
	bool Alt() const {
		return (bLeftAlt || bRightAlt);
	}


	/** Return true if left alt is currently pressed
	  */	
	bool LeftAlt() const {
		return bLeftShift;
	}
	
	/** Return true if right alt is currently pressed
	  */
	bool RightAlt() const {
		return bLeftShift;
	}
	
	/** Poll the state of a key
	  * If the state of the key is currently pressed, mark it as released. 
	  * This method is useful for making the key sensitive to being pressed, but not to being held down.
	  */
	bool Poll(const uint8_t &key);
	
	/** Press a key
	  * If bStreamMode is set, key is added to the stream buffer.
	  */
	void KeyDown(const int32_t& key, const int32_t& mods);
	
	/** Release a key
	  */
	void KeyUp(const int32_t& key, const int32_t& mods);

	/** Get a character from the stream buffer
	  * If the stream buffer is empty, return false.
	  */
	bool Get(char& key);

	/** Reset the stream buffer and all key states
	  */
	void Reset();

	/** Handle OpenGL keyboard presses
	  * @param window Pointer to the GLFW window in which keyboard event occured
	  * @param key The keyboard character which was pressed
	  * @param scancode Platform dependent key scancode
	  * @param action Keyboard event type (GLFW_PRESS GLFW_RELEASE GLFW_REPEAT)
	  * @param mods Specifies whether or not lock keys are active (caps lock, num lock)
	  */
	static void HandleKeys(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);

private:
	uint16_t nCount; ///< Number of standard keyboard keys which are currently pressed

	bool bStreamMode; ///< Flag to set keyboard to behave as stream buffer

	bool bShiftKey;

	bool bCtrlKey;

	bool bAltKey;

	bool bLeftShift; ///< Set if left shift is pressed
	
	bool bLeftCtrl; ///< Set if left ctrl is pressed
	
	bool bLeftAlt; ///< Set if left alt is pressed
	
	bool bRightShift; ///< Set if right shift is pressed
	
	bool bRightCtrl; ///< Set if right ctrl is pressed
	
	bool bRightAlt; ///< Set if right alt is pressed

	std::queue<char> buffer; ///< Text stream buffer

	bool states[256]; ///< States of keyboard keys (true indicates key is down) 

	GLFWwindow* parent; ///< Pointer to parent window
	
	/** Default constructor
	  */
	Keyboard();

	void CheckKeyMods(const int32_t& mods);
	
	/** Press or release a special key (e.g. function key, shift, etc)
	  */
	uint8_t ConvertKey(const int32_t& key, bool bKeyDown);
	
	/** Setup glfw keyboard callback functions
	  */
	void SetupCallbacks(bool bEnable=true);
};

} /* namespace ott */

#endif // #ifndef INPUT_OTT_KEYBOARD_HPP
