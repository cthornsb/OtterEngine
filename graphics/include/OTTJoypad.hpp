#ifndef OTT_JOYPAD_HPP
#define OTT_JOYPAD_HPP

#include <string>
#include <vector>
#include <map>

class GLFWwindow;

enum class GamepadInput{
	A,
	B,
	X,
	Y,
	BACK,
	HOME,
	START,
	DPAD,
	UP,
	RIGHT,
	DOWN,
	LEFT,
	LSTICK,
	LB,
	LT,
	L3,
	RSTICK,
	RB,
	RT,
	R3
};

class AnalogStick{
public:
	float fX; ///< Analog stick x position (-1, 1)
	
	float fY; ///< Analog stick y position (-1, 1)
	
	float fDist; ///< Analog stick squared "distance" from center (
	
	/** Default constructor
	  */
	AnalogStick() = delete;
	
	/** Array index constructor
	  */
	AnalogStick(const unsigned char& xIndex, const unsigned char& yIndex) :
		fX(0.f),
		fY(0.f),
		fDist(0.f),
		nX(xIndex),
		nY(yIndex)
	{
	}

	/** Set analog stick X and Y positions from an input data array
	  */
	void set(const float* data){
		fX = data[nX];
		fY = data[nY];
		fDist = fX * fX + fY * fY;
	}
	
private:
	unsigned char nX; ///< Data array index for X value
	
	unsigned char nY; ///< Data array index for Y value
};

class OTTJoypad{
public:
	/** Copy constructor
	  */
	OTTJoypad(const OTTJoypad&) = delete;
	
	/** Assignment operator
	  */
	OTTJoypad& operator = (const OTTJoypad&) = delete;

	/** Get a reference to the singleton joypad object
	  */
	static OTTJoypad& getInstance();

	/** Set pointer to parent window
	  */
	void setParentWindow(GLFWwindow* ptr){
		parent = ptr;
	}

	/** Enable gamepad callbacks.
	  * Required in order to handle gamepad input.
	  */
	void enable();
	
	/** Disable gamepad callbacks
	  */
	void disable();
	
	/** Return true if no gamepads are currently connected
	  */
	bool empty() const { 
		return (nGamepads == 0); 
	}
	
	/** Check if a gamepad button is currently pressed or an analog stick is away from its neutral position
	  */
	bool poll(const GamepadInput& input);

	/** Search for currently connected gamepads
	  */
	void reset();
	
	/** Check if a gamepad with the specified GLFW ID is currently connected
	  */
	bool status(const int& id) const ;

	/** Return true if the joystick at the specified GLFW ID is currently 
	  * connected and conforms to standard GLFW gamepad layout.
	  * GLFW 3.3 and above only.
	  */
	bool isGamepad(const int& id) const ;

	/** Get the current gamepad state. Joystick at specified GLFW ID must
	  * be connected and must have a standard GLFW gamepad layout.
	  * GLFW 3.3 and above only.
	  */
	bool getGamepadState(const int& id);

	/** Get the current position of the DPad.
	  * Despite the position being read from the device as floating point values, the DPad likely
	  * has no pressure sensitivity and thus X and Y will read back as either -1, 0, or 1.
	  */
	void getDPadPosition(float& x, float& y){
		x = dpad.fX;
		y = dpad.fY;
	}
	
	/** Get the current position of the left analog stick
	  */
	void getLeftStickPosition(float& x, float& y){
		x = leftStick.fX;
		y = leftStick.fY;
	}
	
	/** Get the current position of the right analog stick
	  */
	void getRightStickPosition(float& x, float& y){
		x = rightStick.fX;
		y = rightStick.fY;
	}

	/** Get the current "position" of the left trigger
	  */
	float getLeftTrigger() const {
		return fLeftTrigger;
	}
	
	/** Get the current "position" of the right trigger
	  */
	float getRightTrigger() const {
		return fRightTrigger;
	}

	/** Update the state of all inputs for the most recently connected gamepad
	  */
	void update();

	/** Connect a gamepad.
	  * Should only be called from GLFW gamepad callback function.
	  */
	void connect(const int& id);
	
	/** Disconnect a gamepad.
	  * Should only be called from GLFW gamepad callback function.
	  */
	void disconnect(const int& id);

	/** Print all currently connected gamepads
	  */
	void print();

	/** Handle joystick connected / disconnected events
	  */
	static void joystickCallback(int id, int event);

private:
	class Gamepad{
	public:
		bool bConnected; ///< Set if gamepad is connected
		
		int nID; ///< GLFW gamepad ID number
		
		std::string sName; ///< GLFW gamepad name (GLFW 3.3 or above)
		
		void* jptr; ///< Joypad pointer (GLFW 3.3 or above)
		
		int nButtons; ///< Number of gamepad button states (also contains "hat" buttons)

		int nHats; ///< Number of gamepad hat states (GLFW 3.3 or above)
		
		int nAxes; ///< Number of gamepad axis states
		
		const unsigned char* nButtonStates; ///< Array of all gamepad button states (GLFW_PRESS or GLFW_RELEASE)
		
		const unsigned char* nHatStates; ///< Array of all gamepad hat button states (GLFW 3.3 or above)
		
		const float* fAxisStates; ///< Array of all gamepad analog stick X and Y positions (-1 to 1)

		/** Default constructor
		  */
		Gamepad() :
			bConnected(false),
			nID(-1),
			sName(),
			jptr(0x0),
			nButtons(0),
			nHats(0),
			nAxes(0),
			nButtonStates(0x0),
			nHatStates(0x0),
			fAxisStates(0x0)
		{
		}
		
		/** Return true if this gamepad is currently connected
		  */
		bool isConnected() const {
			return bConnected;
		}
		
		/** Set this gamepad as connected at the specified GLFW ID
		  */
		void connect(const int& id);
		
		/** Set this gamepad as disconnected
		  */
		void disconnect();
		
		/** Print gamepad information
		  */
		void print() const ;
	
		/** Get all gamepad button states from the device
		  */
		bool getJoystickButtons();
		
		/** Get all gamepad hat-button states from the device
		  */
		bool getJoystickHats();

		/** Get all gamepad analog stick positions from the device
		  */
		bool getJoystickAxes();
	};

	int nGamepads; ///< Number of gamepads connected

	std::vector<Gamepad> gamepads; ///< Vector of all possible gamepad connections
	
	AnalogStick leftStick; ///< Left analog stick position
	
	AnalogStick rightStick; ///< Right analog stick position
	
	AnalogStick dpad; ///< Directional pad (DPad) position
	
	float fLeftTrigger; ///< Left trigger pressure (-1 to 1)
	
	float fRightTrigger; ///< Right trigger pressure (-1 to 1)
	
	std::map<GamepadInput, unsigned char> buttonMap; ///< Input map for 360-style gamepad buttons
	
	Gamepad* lastGamepad; ///< Pointer to most recently connected gamepad

	GLFWwindow* parent; ///< Pointer to parent window

	/** Default constructor
	  */
	OTTJoypad();
	
	/** Setup glfw gamepad callback functions
	  */
	void setupCallbacks(bool bEnable=true);
};

#endif

