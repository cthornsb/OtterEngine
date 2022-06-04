#ifndef INPUT_OTT_JOYPAD_HPP
#define INPUT_OTT_JOYPAD_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct GLFWwindow;

namespace ott {

enum class GamepadInput{
	A,
	B,
	X,
	Y,
	Cross,
	Circle,
	Square,
	Triangle,
	Back,
	Guide,
	Start,
	DPad,
	Up,
	Right,
	Down,
	Left,
	LStick,
	LB,
	LT,
	L3,
	RStick,
	RB,
	RT,
	R3
};

class Gamepad;

typedef std::vector<Gamepad*>::iterator GamepadIterator;

class Joypad{
public:
	/** Copy constructor
	  */
	Joypad(const Joypad&) = delete;
	
	/** Assignment operator
	  */
	Joypad& operator = (const Joypad&) = delete;

	/** Get a reference to the singleton joypad object
	  */
	static Joypad& Instance();

	/** Set pointer to parent window
	  */
	void SetParentWindow(GLFWwindow* ptr){
		parent = ptr;
	}

	/** Enable gamepad callbacks.
	  * Required in order to handle gamepad input.
	  */
	void Enable();
	
	/** Disable gamepad callbacks
	  */
	void Disable();
	
	/** Return true if no gamepads are currently connected
	  */
	bool Empty() const { 
		return connected.empty(); 
	}
	
	/** Return true if a gamepad is connected and is in use
	  */
	bool Connected() const;
	
	/** Return true if a gamepad is connected and button data is available for use
	  */
	bool Ready() const;

	/** Get the number of currently valid gamepads which are currently connected
	  */
	int32_t NumberOfGamepads() const {
		return nGamepads;
	}
	
	/** Get the number of currently connected player gamepads
	  */
	int32_t NumberOfPlayers() const {
		return nPlayers;
	}

	/** Poll the state of a gamepad button.
	  * If the button is currently pressed, mark it as released.
	  */
	bool Poll(const GamepadInput& input);
	
	/** Check if a gamepad button is currently pressed or an analog stick is away from its neutral position
	  */
	bool Check(const GamepadInput& input);

	/** Search for currently connected gamepads
	  */
	void Reset();
	
	/** Check if a gamepad with the specified GLFW ID is currently connected
	  */
	bool Status(const int32_t& id) const ;

	/** Return true if the joystick at the specified GLFW ID is currently 
	  * connected and conforms to standard GLFW gamepad layout.
	  * GLFW 3.3 and above only.
	  */
	bool IsGamepad(const int32_t& id) const ;

	/** Get the current position of the DPad.
	  * Despite the position being read from the device as floating point values, the DPad likely
	  * has no pressure sensitivity and thus X and Y will read back as either -1, 0, or 1.
	  * @return True if a gamepad is connected
	  */
	bool DPadPosition(float& x, float& y) const ;
	
	/** Get the current position of the left analog stick
	  * @return True if a gamepad is connected
	  */
	bool LeftStickPosition(float& x, float& y) const ;
	
	/** Get the change in position of the left analog stick since the last update
	  * @return True if a gamepad is connected
	  */
	bool LeftStickDeltaPosition(float& dx, float& dy) const ;

	/** Get the current position of the right analog stick
	  * @return True if a gamepad is connected
	  */
	bool RightStickPosition(float& x, float& y) const ;

	/** Get the change in position of the right analog stick since the last update
	  * @return True if a gamepad is connected
	  */
	bool RightStickDeltaPosition(float& dx, float& dy) const ;

	/** Get the current "position" of the left trigger
	  */
	float LeftTrigger() const ;
	
	/** Get the current "position" of the right trigger
	  */
	float RightTrigger() const ;

	/** Set the pressure threshold for which the directional pad (DPad) is considered "pressed" (default is 0.05)
	  * @param sens Pressure threshold in the range 0 to 2, where 0 is not pressed and 2 is fully pressed
	  */
	void SetDPadThreshold(const float& sens) {
		fDPadThreshold = sens;
	}

	/** Set the displacement threshold for which the left analog stick is considered to have been moved from its central position (default is 0.05)
	  * @param sens The analog stick's displacement from its central position in the range 0 to 2 where 0 is the central position and 2 is its maximum displacement
	  */
	void SetLeftStickThreshold(const float& sens) {
		fLeftStickThreshold = sens;
	}

	/** Set the displacement threshold for which the right analog stick is considered to have been moved from its central position (default is 0.05)
	  * @param sens The analog stick's displacement from its central position in the range 0 to 2 where 0 is the central position and 2 is its maximum displacement
	  */
	void SetRightStickThreshold(const float& sens) {
		fRightStickThreshold = sens;
	}

	/** Set the pressure threshold for which the left trigger is considered "pressed" (default is 0.05)
	  * @param sens Pressure threshold in the range 0 to 2, where 0 is not pressed and 2 is fully pressed
	  */
	void SetLeftTriggerThreshold(const float& sens) {
		fLeftTriggerThreshold = 1.f - sens;
	}

	/** Set the pressure threshold for which the right trigger is considered "pressed" (default is 0.05)
	  * @param sens Pressure threshold in the range 0 to 2, where 0 is not pressed and 2 is fully pressed
	  */
	void SetRightTriggerThreshold(const float& sens) {
		fRightTriggerThreshold = 1.f - sens;
	}

	/** Calibrate the analog sticks of the the currently active gamepad.
	  * @param cycles Number of gamepad update cycles to perform
	  */
	bool Calibrate(const int32_t& cycles = 10);

	/** Update the state of all inputs for the most recently connected gamepad
	  */
	void Update();

	/** Connect a gamepad.
	  * Should only be called from GLFW gamepad callback function.
	  */
	void Connect(const int32_t& id);
	
	/** Disconnect a gamepad.
	  * Should only be called from GLFW gamepad callback function.
	  */
	void Disconnect(const int32_t& id);
	
	/** Change the currently selected gamepad (assuming more than one are connected).
	  * If specified ID is < 0, the next available gamepad is selected.
	  * @return True if the active gamepad was changed successfully
	  */
	bool ChangeActiveGamepad(const int32_t& id=-1);

	/** Change the currently selected player's gamepad (assuming more than one are connected).
	  * Player indicies start at zero. Each new gamepad connection increments the player number by 1.
	  * @return True if the gamepad was changed successfully
	  */
	bool ChangeActivePlayer(const int32_t& player);

	/** Re-order player ID numbers to fill any gaps caused by disconnections
	  */
	void ReorderPlayers();

	/** Print all currently connected gamepads
	  */
	void Print();

	/** Find a gamepad button by name.
	  * @param name The name of a gamepad button. Must be one of the following lowercase strings; a, b, x, y, lb, rb, back, 
	  *  start, guide, l3, r3, up, right, down, left [or for PS-like controllers; cross, circle, square, triangle].
	  * @param button Returned gamepad button, if button name found successfully
	  * @return True if the button with the specified name is found
	  */
	bool FindNamedButton(const std::string& name, GamepadInput& button) const ;

	/** Handle joystick connected / disconnected events
	  */
	static void JoystickCallback(int32_t id, int32_t event);

private:
	bool bConnected; ///< Set if a gamepad is connected and is in use

	int32_t nGamepads; ///< Number of gamepads connected

	int32_t nPlayers; ///< Number of connected player gamepads

	float fDPadThreshold; // 0.1

	float fLeftStickThreshold; // 0.1

	float fRightStickThreshold; // 0.1

	float fLeftTriggerThreshold; // 0.9 = 1.0 - input

	float fRightTriggerThreshold; // 0.9 = 1.0 - input

	std::unique_ptr<Gamepad[]> gamepads; ///< Array of all possible gamepad connections
	
	std::vector<Gamepad*> connected; ///< Vector of all connected gamepads
	
	std::unordered_map<GamepadInput, std::pair<uint8_t, std::string> > buttonMap; ///< Input map for 360-style gamepad buttons
	
	Gamepad* lastGamepad; ///< Pointer to most recently connected gamepad

	GLFWwindow* parent; ///< Pointer to parent window

	/** Default constructor
	  */
	Joypad();
	
	/** Setup glfw gamepad callback functions
	  */
	void SetupCallbacks(bool bEnable=true);
	
	/** Search the list of connected gamepads for one with matching id
	  * @return The iterator of the gamepad with the specified id number, or connected.end() if the specified id number was not found.
	  */
	GamepadIterator FindGamepad(const int32_t& id);

	/** Search the list of connected gamepads for one with matching player id number
	  * @return The iterator of the gamepad with the specified player id, or connected.end() if a matching gamepad was not found.
	  */
	GamepadIterator FindPlayer(const int32_t& player);

	/** Calibrate the analog sticks of a gamepad.
	  * Calibration readings are averaged for a specified number of cycles, sleeping a short time between each update.
	  */
	void Calibrate(Gamepad* pad, const int32_t& cycles=10);
};

} /* namespace ott */

#endif // #ifndef INPUT_OTT_JOYPAD_HPP
