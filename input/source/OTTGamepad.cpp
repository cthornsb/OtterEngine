#include <GLFW/glfw3.h>

#include "OTTGamepad.hpp"

#include <iostream>

namespace ott {
	GLFWgamepadstate gButtonStates; ///< Gamepad button and analog stick states at most recent update() call
	GLFWgamepadstate gPreviousStates; ///< Gamepad button and analog stick states at previous update() call
}

bool ott::Gamepad::Connect(const int32_t& id) {
	nID = id;
	bConnected = true;
#ifdef GLFW_VERSION_3_3
	sName = std::string(glfwGetJoystickName(id)); // GLFW 3.3
	jptr = glfwGetJoystickUserPointer(id); // GLFW 3.3
#endif // ifdef GLFW_VERSION_3_3
	return true;
}

void ott::Gamepad::Disconnect() {
	bConnected = false;
	bValidGamepad = true;
	sName = "";
	nID = -1;
	nPlayer = -1;
	nButtons = 0;
	nHats = 0;
	nAxes = 0;
	jptr = 0x0;
	nButtonStates = 0x0;
	nHatStates = 0x0;
	fAxisStates = 0x0;
	for (int32_t i = 0; i < 15; i++)
		bToggleStates[i] = false;
}

bool ott::Gamepad::Poll(const uint8_t& input) {
	if (bToggleStates[input]) {
		bToggleStates[input] = false;
		return true;
	}
	return false;
}

bool ott::Gamepad::Check(const uint8_t& input) {
	return bToggleStates[input];
}

bool ott::Gamepad::Update() {
#ifdef GLFW_VERSION_3_3
	glfwGetGamepadState(nID, &ott::gButtonStates); // GLFW 3.3
#else
	if (!bValidGamepad)
		return false;
	if (this->JoystickButtons() == false) {
		bValidGamepad = false;
		return false;
	}
	this->JoystickHats();
	this->JoystickAxes();
#endif // ifdef GLFW_VERSION_3_3
	this->UpdateButtonData();
	return true;
}

void ott::Gamepad::Print() const {
	std::cout << "  Gamepad id=" << nID << ", name=\"" << sName << "\"" << std::endl;
}

bool ott::Gamepad::JoystickButtons() {
	nButtonStates = glfwGetJoystickButtons(nID, &nButtons); // GLFW 3.0
	return (nButtons > 0);
}

bool ott::Gamepad::JoystickHats() {
#ifdef GLFW_VERSION_3_3
	nHatStates = glfwGetJoystickHats(nID, &nHats); // GLFW 3.3
	return (nHats > 0);
#else
	return false;
#endif // ifdef GLFW_VERSION_3_3
}

bool ott::Gamepad::JoystickAxes() {
	fAxisStates = glfwGetJoystickAxes(nID, &nAxes); // GLFW 3.0
	return (nAxes > 0);
}

void ott::Gamepad::UpdateButtonData() {
#ifdef GLFW_VERSION_3_3
	leftStick.set(
		ott::gButtonStates.axes[GLFW_GAMEPAD_AXIS_LEFT_X],
		ott::gButtonStates.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]
	);
	rightStick.set(
		ott::gButtonStates.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
		ott::gButtonStates.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]
	);
	fLeftTrigger = ott::gButtonStates.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
	fRightTrigger = ott::gButtonStates.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
	for (int32_t i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
		if (ott::gButtonStates.buttons[i] != ott::gPreviousStates.buttons[i]) { // Button changed states
			if (ott::gButtonStates.buttons[i] == GLFW_PRESS) // Button pressed
				bToggleStates[i] = true;
			else // Button released
				bToggleStates[i] = false;
		}
		ott::gPreviousStates.buttons[i] = ott::gButtonStates.buttons[i];
	}
#else
	leftStick.Set(fAxisStates);
	rightStick.Set(fAxisStates);
	dpad.Set(fAxisStates);
	fLeftTrigger = fAxisStates[2];
	fRightTrigger = fAxisStates[5];
	for (int32_t i = 0; i < nButtons; i++) {
		if (nButtonStates[i] != ott::gPreviousStates.buttons[i]) { // Button changed states
			if (nButtonStates[i] == GLFW_PRESS) // Button pressed
				bToggleStates[i] = true;
			else // Button released
				bToggleStates[i] = false;
		}
		ott::gPreviousStates.buttons[i] = nButtonStates[i];
	}
#endif // ifdef GLFW_VERSION_3_3
	bGood = true;
}
