#ifndef INPUT_OTT_GAMEPAD_HPP
#define INPUT_OTT_GAMEPAD_HPP

#include "OTTAnalogStick.hpp"

#include <cstdint>
#include <string>

namespace ott {

class Gamepad {
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
		bToggleStates{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	{
	}

	/** GLFW gamepad id constructor
	  */
	Gamepad(const int32_t& id) :
		Gamepad()
	{
		nID = id;
	}

	/** Destructor
	  */
	~Gamepad() {
	}

	/** Return true if this gamepad is currently connected
	  */
	bool Connected() const {
		return bConnected;
	}

	/** Return true if connected device is a valid gamepad (with at least one button)
	  */
	bool Valid() const {
		return bValidGamepad;
	}

	/** Return true if gamepad data is available for use
	  */
	bool Good() const {
		return bGood;
	}

	/** Get GLFW gamepad ID number
	  */
	int32_t Id() const {
		return nID;
	}

	/** Get gamepad player number
	  */
	int32_t PlayerNumber() const {
		return nPlayer;
	}

	/** Get pointer to the left analog stick
	  */
	AnalogStick* LeftAnalogStick() {
		return &leftStick;
	}

	/** Get pointer to the right analog stick
	  */
	AnalogStick* RightAnalogStick() {
		return &rightStick;
	}

	/** Get pointer to the directional-pad (DPad)
	  */
	AnalogStick* DPad() {
		return &dpad;
	}

	/** Get the left trigger position
	  */
	float LeftTriggerPosition() const {
		return fLeftTrigger;
	}

	/** Get the right trigger position
	  */
	float RightTriggerPosition() const {
		return fRightTrigger;
	}

	/** Set GLFW gamepad ID number
	  */
	void SetId(const int32_t& id) {
		nID = id;
	}

	/** Set gamepad player number
	  */
	void SetPlayerNumber(const int32_t& player) {
		nPlayer = player;
	}

	/** Set this gamepad as connected at the specified GLFW ID
	  */
	bool Connect(const int32_t& id);

	/** Disconnect this gamepad
	  */
	void Disconnect();

	/** Retrieve all button and analog stick data from device
	  */
	bool Update();

	/** Poll the state of a gamepad button.
	  * If the button is currently pressed, mark it as released.
	  */
	bool Poll(const uint8_t& input);

	/** Check if a gamepad button is currently pressed or an analog stick is away from its neutral position
	  */
	bool Check(const uint8_t& input);

	/** Print gamepad information
	  */
	void Print() const;

protected:
	bool bConnected; ///< Set if gamepad is connected

	bool bValidGamepad; ///< Set if connected device is a valid gamepad

	bool bGood; ///< Set when a valid gamepad is connected and data has been read from it

	int32_t nID; ///< GLFW gamepad ID number

	int32_t nPlayer; ///< Player ID number

	std::string sName; ///< GLFW gamepad name (GLFW 3.3 or above)

	void* jptr; ///< Joypad pointer (GLFW 3.3 or above)

	int32_t nButtons; ///< Number of gamepad button states (also contains "hat" buttons)

	int32_t nHats; ///< Number of gamepad hat states (GLFW 3.3 or above)

	int32_t nAxes; ///< Number of gamepad axis states

	const uint8_t* nButtonStates; ///< Array of all gamepad button states (GLFW_PRESS or GLFW_RELEASE)

	const uint8_t* nHatStates; ///< Array of all gamepad hat button states (GLFW 3.3 or above)

	const float* fAxisStates; ///< Array of all gamepad analog stick X and Y positions (-1 to 1)

	AnalogStick leftStick; ///< Left analog stick position

	AnalogStick rightStick; ///< Right analog stick position

	AnalogStick dpad; ///< Directional pad (DPad) position

	float fLeftTrigger; ///< Left trigger pressure (-1 to 1)

	float fRightTrigger; ///< Right trigger pressure (-1 to 1)

	bool bToggleStates[15]; ///< Gamepad button states toggled when button is pressed

	/** Get all gamepad button states from the device
	  */
	bool JoystickButtons();

	/** Get all gamepad hat-button states from the device
	  */
	bool JoystickHats();

	/** Get all gamepad analog stick positions from the device
	  */
	bool JoystickAxes();

	/** Called whenever the user calls update() method
	  */
	void UpdateButtonData();
};

} /* namespace ott */

#endif // #ifndef INPUT_OTT_GAMEPAD_HPP
