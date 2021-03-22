#ifndef OTT_JOYPAD_HPP
#define OTT_JOYPAD_HPP

#include <string>
#include <vector>
#include <unordered_map>

class GLFWwindow;

enum class GamepadInput{
	A,
	B,
	X,
	Y,
	CROSS,
	CIRCLE,
	SQUARE,
	TRIANGLE,
	BACK,
	GUIDE,
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

#ifndef GLFW_VERSION_3_3

	/** Simple data struct defined for backwards compatibility with GLFW versions prior to 3.3
	  */
	struct GLFWgamepadstate{
		unsigned char buttons[15];
		float axes[6];
		
		GLFWgamepadstate() :
			buttons{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			axes{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f }
		{
		}
	};
	
	#define GLFW_GAMEPAD_BUTTON_A 0
	#define GLFW_GAMEPAD_BUTTON_B 1
	#define GLFW_GAMEPAD_BUTTON_X 2
	#define GLFW_GAMEPAD_BUTTON_Y 3
	#define GLFW_GAMEPAD_BUTTON_LEFT_BUMPER 4
	#define GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER 5
	#define GLFW_GAMEPAD_BUTTON_BACK 6
	#define GLFW_GAMEPAD_BUTTON_START 7
	#define GLFW_GAMEPAD_BUTTON_GUIDE 8
	#define GLFW_GAMEPAD_BUTTON_LEFT_THUMB 9
	#define GLFW_GAMEPAD_BUTTON_RIGHT_THUMB 10
	#define GLFW_GAMEPAD_BUTTON_DPAD_UP 11
	#define GLFW_GAMEPAD_BUTTON_DPAD_RIGHT 12
	#define GLFW_GAMEPAD_BUTTON_DPAD_DOWN 13
	#define GLFW_GAMEPAD_BUTTON_DPAD_LEFT 14

	#define GLFW_GAMEPAD_BUTTON_CROSS GLFW_GAMEPAD_BUTTON_A
	#define GLFW_GAMEPAD_BUTTON_CIRCLE GLFW_GAMEPAD_BUTTON_B
	#define GLFW_GAMEPAD_BUTTON_SQUARE GLFW_GAMEPAD_BUTTON_X
	#define GLFW_GAMEPAD_BUTTON_TRIANGLE GLFW_GAMEPAD_BUTTON_Y
	
	#define	GLFW_GAMEPAD_AXIS_LEFT_X 0
	#define GLFW_GAMEPAD_AXIS_LEFT_Y 1
	#define	GLFW_GAMEPAD_AXIS_RIGHT_X 2
	#define	GLFW_GAMEPAD_AXIS_RIGHT_Y 3
	#define	GLFW_GAMEPAD_AXIS_LEFT_TRIGGER 4
	#define	GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER 5
	
#endif // ifndef GLFW_VERSION_3_3

class AnalogStick{
public:
	float fX; ///< Analog stick x position (-1, 1)
	
	float fY; ///< Analog stick y position (-1, 1)
	
	float fDX; ///< Change in analog x position since last position update
	
	float fDY; ///< Change in analog y position since last position update
	
	float fDist; ///< Analog stick squared "distance" from center (
	
	/** Default constructor
	  */
	AnalogStick() = delete;
	
	/** Array index constructor
	  */
	AnalogStick(const unsigned char& xIndex, const unsigned char& yIndex) :
		fX(0.f),
		fY(0.f),
		fDX(0.f),
		fDY(0.f),
		fDist(0.f),
		nX(xIndex),
		nY(yIndex)
	{
	}

	/** Set analog stick X and Y positions from an input data array
	  */
	void set(const float* data){
		fDX = data[nX] - fX;
		fDY = data[nY] - fY;
		fX = data[nX];
		fY = data[nY];
		fDist = fX * fX + fY * fY;
	}
	
	/** Set analog stick X and Y positions from explicit floating point values
	  */
	void set(const float& x, const float& y){
		fDX = x - fX;
		fDY = y - fY;
		fX = x;
		fY = y;
		fDist = fX * fX + fY * fY;
	}
	
private:
	unsigned char nX; ///< Data array index for X value
	
	unsigned char nY; ///< Data array index for Y value
};

class LogicalGamepad{
public:
	/** Default constructor
	  */
	LogicalGamepad() = delete;
	
	/** GLFW gamepad id constructor
	  */
	LogicalGamepad(const int& id) :
		bConnected(false),
		nID(id),
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
	
	/** Destructor
	  */
	~LogicalGamepad(){ 
	}
	
	/** GLFW gamepad id number equality operator
	  */
	bool operator == (const int& id) const {
		return (nID == id);
	}
	
	/** Pointer equality operator
	  */
	bool operator == (LogicalGamepad* rhs) const {
		return (rhs == this);
	}
	
	/** Return true if this gamepad is currently connected
	  */
	bool isConnected() const {
		return bConnected;
	}
	
	/** Get GLFW gamepad ID number
	  */
	int getID() const {
		return nID;
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

	/** Retrieve all button and analog stick data from device
	  */
	void update();

protected:
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

	/** Get all gamepad button states from the device
	  */
	bool getJoystickButtons();
	
	/** Get all gamepad hat-button states from the device
	  */
	bool getJoystickHats();

	/** Get all gamepad analog stick positions from the device
	  */
	bool getJoystickAxes();
	
	/** Called whenever the user calls update() method
	  */
	virtual void onUserUpdate() { }
};

class Gamepad : public LogicalGamepad {
public:
	/** Default constructor
	  */
	Gamepad() = delete;
	
	/** GLFW gamepad id constructor
	  */
	Gamepad(const int& id);
	
	/** Destructor
	  */
	~Gamepad(){
	}
	
	/** Return true if gamepad data is available for use
	  */
	bool isGood() const {
		return bGood;
	}
	
	/** Get pointer to the left analog stick
	  */
	AnalogStick* getLeftAnalogStick() {
		return &leftStick;
	}
	
	/** Get pointer to the right analog stick
	  */
	AnalogStick* getRightAnalogStick() {
		return &rightStick;
	}
	
	/** Get pointer to the directional-pad (DPad)
	  */
	AnalogStick* getDPad() {
		return &dpad;
	}
	
	/** Get the left trigger position
	  */
	float getLeftTriggerPosition() const {
		return fLeftTrigger;
	}
	
	/** Get the right trigger position
	  */
	float getRightTriggerPosition() const {
		return fRightTrigger;
	}
	
	/** 
	  */
	bool poll(const unsigned char& input);
	
	/**
	  */
	bool check(const unsigned char& input);

	void press(const unsigned char& input);
	
private:
	bool bGood;

	AnalogStick leftStick; ///< Left analog stick position
	
	AnalogStick rightStick; ///< Right analog stick position
	
	AnalogStick dpad; ///< Directional pad (DPad) position
	
	float fLeftTrigger; ///< Left trigger pressure (-1 to 1)
	
	float fRightTrigger; ///< Right trigger pressure (-1 to 1)
	
	GLFWgamepadstate buttonStates;
	
	GLFWgamepadstate prevStates;
	
	bool bToggleStates[15]; ///< 
	
	void onUserUpdate() override ;
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
		return gamepads.empty(); 
	}
	
	/** Return true if a gamepad is connected and is in use
	  */
	bool isConnected() const {
		return (bConnected && lastGamepad);
	}
	
	/** Return true if a gamepad is connected and button data is available for use
	  */
	bool isReady() const {
		return (lastGamepad && lastGamepad->isGood());
	}

	/** Poll the state of a gamepad button.
	  * If the button is currently pressed, mark it as released.
	  */
	bool poll(const GamepadInput& input);
	
	/** Check if a gamepad button is currently pressed or an analog stick is away from its neutral position
	  */
	bool check(const GamepadInput& input);

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

	/** Get the current position of the DPad.
	  * Despite the position being read from the device as floating point values, the DPad likely
	  * has no pressure sensitivity and thus X and Y will read back as either -1, 0, or 1.
	  * @return True if a gamepad is connected and input data is available
	  */
	bool getDPadPosition(float& x, float& y) const {
		if(!lastGamepad || !lastGamepad->isGood())
			return false;
		x = lastGamepad->getDPad()->fX;
		y = lastGamepad->getDPad()->fY;
		return true;
	}
	
	/** Get the current position of the left analog stick
	  * @return True if a gamepad is connected and input data is available
	  */
	bool getLeftStickPosition(float& x, float& y) const {
		if(!lastGamepad || !lastGamepad->isGood())
			return false;
		x = lastGamepad->getLeftAnalogStick()->fX;
		y = lastGamepad->getLeftAnalogStick()->fY;
		return true;
	}

	/** Get the current position of the right analog stick
	  * @return True if a gamepad is connected and input data is available
	  */
	bool getRightStickPosition(float& x, float& y) const {
		if(!lastGamepad || !lastGamepad->isGood())
			return false;
		x = lastGamepad->getRightAnalogStick()->fX;
		y = lastGamepad->getRightAnalogStick()->fY;
		return true;
	}

	/** Get the current "position" of the left trigger
	  */
	float getLeftTrigger() const {
		if(lastGamepad)
			return lastGamepad->getLeftTriggerPosition();
		return 0.f;
	}
	
	/** Get the current "position" of the right trigger
	  */
	float getRightTrigger() const {
		if(lastGamepad)
			return lastGamepad->getRightTriggerPosition();
		return 0.f;
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
	
	/** Change the currently selected gamepad (assuming more than one are connected).
	  * If specified ID is < 0, the next available gamepad is selected.
	  * @return True if the active gamepad was changed successfully
	  */
	bool changeActiveGamepad(const int& id=-1);

	/** Print all currently connected gamepads
	  */
	void print();

	/** Handle joystick connected / disconnected events
	  */
	static void joystickCallback(int id, int event);

private:
	bool bConnected; ///< Set if a gamepad is connected and is in use

	int nGamepads; ///< Number of gamepads connected

	std::vector<Gamepad> gamepads; ///< Vector of all possible gamepad connections
	
	std::unordered_map<GamepadInput, unsigned char> buttonMap; ///< Input map for 360-style gamepad buttons
	
	Gamepad* lastGamepad; ///< Pointer to most recently connected gamepad

	GLFWwindow* parent; ///< Pointer to parent window

	/** Default constructor
	  */
	OTTJoypad();
	
	/** Setup glfw gamepad callback functions
	  */
	void setupCallbacks(bool bEnable=true);
	
	/** Search the list of connected gamepads for one with matching id
	  * @return The iterator of the gamepad with the specified id number, or gamepads.end() if the specified id number was not found.
	  */
	std::vector<Gamepad>::iterator findGamepad(const int& id);

	/** Search the list of connected gamepads for one with matching pointer address
	  * @return The iterator of the gamepad with the specified pointer address, or gamepads.end() if a matching gamepad was not found.
	  */
	std::vector<Gamepad>::iterator findGamepad(LogicalGamepad* id);
};

#endif

