#ifndef OTT_JOYPAD_HPP
#define OTT_JOYPAD_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

struct GLFWwindow;

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

#if (GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3)
	// GLFW version is at least 3.3
	#define GLFW_VERSION_3_3
#endif // if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3

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
	
	float fDist; ///< Analog stick squared "distance" from center
	
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
		nY(yIndex),
		fOffsetX(0.f),
		fOffsetY(0.f)
	{
	}

	/** Set analog stick X and Y positions from an input data array
	  */
	void set(const float* data){
		fDX = (data[nX] - fOffsetX) - fX;
		fDY = (data[nY] - fOffsetY) - fY;
		fX = (data[nX] - fOffsetX);
		fY = (data[nY] - fOffsetY);
		fDist = fX * fX + fY * fY;
	}
	
	/** Set analog stick X and Y positions from explicit floating point values
	  */
	void set(const float& x, const float& y){
		fDX = (x - fOffsetX) - fX;
		fDY = (y - fOffsetX) - fY;
		fX = (x - fOffsetX);
		fY = (y - fOffsetY);
		fDist = fX * fX + fY * fY;
	}

	/** Set analog stick center point offset calibration
	  */
	void setCalibration(const float& x, const float& y) {
		fOffsetX = x;
		fOffsetY = y;
	}
	
private:
	unsigned char nX; ///< Data array index for X value
	
	unsigned char nY; ///< Data array index for Y value

	float fOffsetX; ///< Analog stick center x calibration offset

	float fOffsetY; ///< Analog stick center y calibration offset
};

class Gamepad{
public:
	/** Default constructor
	  */
	Gamepad() :
		bConnected(false),
		bValidGamepad(true),
		bGood(false),
		nID(-1),
		nPlayer(-1),
		sName(),
		jptr(0x0),
		nButtons(0),
		nHats(0),
		nAxes(0),
		nButtonStates(0x0),
		nHatStates(0x0),
		fAxisStates(0x0),
		leftStick(0, 1),
		rightStick(3, 4),
		dpad(6, 7),
		fLeftTrigger(0.f),
		fRightTrigger(0.f),
		buttonStates(),
		prevStates(),
		bToggleStates{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	{
	}
	
	/** GLFW gamepad id constructor
	  */
	Gamepad(const int& id) :
		Gamepad()
	{
		nID = id;
	}
	
	/** Destructor
	  */
	~Gamepad(){ 
	}
	
	/** Return true if this gamepad is currently connected
	  */
	bool isConnected() const {
		return bConnected;
	}
	
	/** Return true if connected device is a valid gamepad (with at least one button)
	  */
	bool isValid() const {
		return bValidGamepad;
	}

	/** Return true if gamepad data is available for use
	  */
	bool isGood() const {
		return bGood;
	}
	
	/** Get GLFW gamepad ID number
	  */
	int getID() const {
		return nID;
	}
	
	/** Get gamepad player number
	  */
	int getPlayerNumber() const {
		return nPlayer;	
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
	
	/** Set GLFW gamepad ID number
	  */
	void setID(const int& id){
		nID = id;
	}
	
	/** Set gamepad player number
	  */
	void setPlayerNumber(const int& player){
		nPlayer = player;
	}
	
	/** Set this gamepad as connected at the specified GLFW ID
	  */
	bool connect(const int& id);
	
	/** Disconnect this gamepad
	  */
	void disconnect();
	
	/** Retrieve all button and analog stick data from device
	  */
	bool update();

	/** Poll the state of a gamepad button.
	  * If the button is currently pressed, mark it as released.
	  */
	bool poll(const unsigned char& input);
	
	/** Check if a gamepad button is currently pressed or an analog stick is away from its neutral position
	  */
	bool check(const unsigned char& input);

	/** Print gamepad information
	  */
	void print() const ;

protected:
	bool bConnected; ///< Set if gamepad is connected
	
	bool bValidGamepad; ///< Set if connected device is a valid gamepad
	
	bool bGood; ///< Set when a valid gamepad is connected and data has been read from it
	
	int nID; ///< GLFW gamepad ID number
	
	int nPlayer; ///< Player ID number
	
	std::string sName; ///< GLFW gamepad name (GLFW 3.3 or above)
	
	void* jptr; ///< Joypad pointer (GLFW 3.3 or above)
	
	int nButtons; ///< Number of gamepad button states (also contains "hat" buttons)

	int nHats; ///< Number of gamepad hat states (GLFW 3.3 or above)
	
	int nAxes; ///< Number of gamepad axis states
	
	const unsigned char* nButtonStates; ///< Array of all gamepad button states (GLFW_PRESS or GLFW_RELEASE)
	
	const unsigned char* nHatStates; ///< Array of all gamepad hat button states (GLFW 3.3 or above)
	
	const float* fAxisStates; ///< Array of all gamepad analog stick X and Y positions (-1 to 1)

	AnalogStick leftStick; ///< Left analog stick position
	
	AnalogStick rightStick; ///< Right analog stick position
	
	AnalogStick dpad; ///< Directional pad (DPad) position
	
	float fLeftTrigger; ///< Left trigger pressure (-1 to 1)
	
	float fRightTrigger; ///< Right trigger pressure (-1 to 1)
	
	GLFWgamepadstate buttonStates; ///< Gamepad button and analog stick states at most recent update() call
	
	GLFWgamepadstate prevStates; ///< Gamepad button and analog stick states at previous update() call
	
	bool bToggleStates[15]; ///< Gamepad button states toggled when button is pressed

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
	void updateButtonData();
};

typedef std::vector<Gamepad*>::iterator GamepadIterator;

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
		return connected.empty(); 
	}
	
	/** Return true if a gamepad is connected and is in use
	  */
	bool isConnected() const {
		return (lastGamepad && lastGamepad->isConnected());
	}
	
	/** Return true if a gamepad is connected and button data is available for use
	  */
	bool isReady() const {
		return (lastGamepad && lastGamepad->isGood());
	}

	/** Get the number of currently valid gamepads which are currently connected
	  */
	int getNumberOfGamepads() const {
		return nGamepads;
	}
	
	/** Get the number of currently connected player gamepads
	  */
	int getNumberOfPlayers() const {
		return nPlayers;
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
	  * @return True if a gamepad is connected
	  */
	bool getDPadPosition(float& x, float& y) const ;
	
	/** Get the current position of the left analog stick
	  * @return True if a gamepad is connected
	  */
	bool getLeftStickPosition(float& x, float& y) const ;
	
	/** Get the change in position of the left analog stick since the last update
	  * @return True if a gamepad is connected
	  */
	bool getLeftStickDeltaPosition(float& dx, float& dy) const ;

	/** Get the current position of the right analog stick
	  * @return True if a gamepad is connected
	  */
	bool getRightStickPosition(float& x, float& y) const ;

	/** Get the change in position of the right analog stick since the last update
	  * @return True if a gamepad is connected
	  */
	bool getRightStickDeltaPosition(float& dx, float& dy) const ;

	/** Get the current "position" of the left trigger
	  */
	float getLeftTrigger() const ;
	
	/** Get the current "position" of the right trigger
	  */
	float getRightTrigger() const ;

	/** Set the pressure threshold for which the directional pad (DPad) is considered "pressed" (default is 0.05)
	  * @param sens Pressure threshold in the range 0 to 2, where 0 is not pressed and 2 is fully pressed
	  */
	void setDPadThreshold(const float& sens) {
		fDPadThreshold = sens;
	}

	/** Set the displacement threshold for which the left analog stick is considered to have been moved from its central position (default is 0.05)
	  * @param sens The analog stick's displacement from its central position in the range 0 to 2 where 0 is the central position and 2 is its maximum displacement
	  */
	void setLeftStickThreshold(const float& sens) {
		fLeftStickThreshold = sens;
	}

	/** Set the displacement threshold for which the right analog stick is considered to have been moved from its central position (default is 0.05)
	  * @param sens The analog stick's displacement from its central position in the range 0 to 2 where 0 is the central position and 2 is its maximum displacement
	  */
	void setRightStickThreshold(const float& sens) {
		fRightStickThreshold = sens;
	}

	/** Set the pressure threshold for which the left trigger is considered "pressed" (default is 0.05)
	  * @param sens Pressure threshold in the range 0 to 2, where 0 is not pressed and 2 is fully pressed
	  */
	void setLeftTriggerThreshold(const float& sens) {
		fLeftTriggerThreshold = 1.f - sens;
	}

	/** Set the pressure threshold for which the right trigger is considered "pressed" (default is 0.05)
	  * @param sens Pressure threshold in the range 0 to 2, where 0 is not pressed and 2 is fully pressed
	  */
	void setRightTriggerThreshold(const float& sens) {
		fRightTriggerThreshold = 1.f - sens;
	}

	/** Calibrate the analog sticks of the the currently active gamepad.
	  * @param cycles Number of gamepad update cycles to perform
	  */
	bool calibrate(const int& cycles = 10);

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

	/** Change the currently selected player's gamepad (assuming more than one are connected).
	  * Player indicies start at zero. Each new gamepad connection increments the player number by 1.
	  * @return True if the gamepad was changed successfully
	  */
	bool changeActivePlayer(const int& player);

	/** Re-order player ID numbers to fill any gaps caused by disconnections
	  */
	void reorderPlayers();

	/** Print all currently connected gamepads
	  */
	void print();

	/** Find a gamepad button by name.
	  * @param name The name of a gamepad button. Must be one of the following lowercase strings; a, b, x, y, lb, rb, back, 
	  *  start, guide, l3, r3, up, right, down, left [or for PS-like controllers; cross, circle, square, triangle].
	  * @param button Returned gamepad button, if button name found successfully
	  * @return True if the button with the specified name is found
	  */
	bool findNamedButton(const std::string& name, GamepadInput& button) const ;

	/** Handle joystick connected / disconnected events
	  */
	static void joystickCallback(int id, int event);

private:
	bool bConnected; ///< Set if a gamepad is connected and is in use

	int nGamepads; ///< Number of gamepads connected

	int nPlayers; ///< Number of connected player gamepads

	float fDPadThreshold; // 0.1

	float fLeftStickThreshold; // 0.1

	float fRightStickThreshold; // 0.1

	float fLeftTriggerThreshold; // 0.9 = 1.0 - input

	float fRightTriggerThreshold; // 0.9 = 1.0 - input

	std::unique_ptr<Gamepad[]> gamepads; ///< Array of all possible gamepad connections
	
	std::vector<Gamepad*> connected; ///< Vector of all connected gamepads
	
	std::unordered_map<GamepadInput, std::pair<unsigned char, std::string> > buttonMap; ///< Input map for 360-style gamepad buttons
	
	Gamepad* lastGamepad; ///< Pointer to most recently connected gamepad

	GLFWwindow* parent; ///< Pointer to parent window

	/** Default constructor
	  */
	OTTJoypad();
	
	/** Setup glfw gamepad callback functions
	  */
	void setupCallbacks(bool bEnable=true);
	
	/** Search the list of connected gamepads for one with matching id
	  * @return The iterator of the gamepad with the specified id number, or connected.end() if the specified id number was not found.
	  */
	GamepadIterator findGamepad(const int& id);

	/** Search the list of connected gamepads for one with matching player id number
	  * @return The iterator of the gamepad with the specified player id, or connected.end() if a matching gamepad was not found.
	  */
	GamepadIterator findPlayer(const int& player);

	/** Calibrate the analog sticks of a gamepad.
	  * Calibration readings are averaged for a specified number of cycles, sleeping a short time between each update.
	  */
	void calibrate(Gamepad* pad, const int& cycles=10);
};

#endif

