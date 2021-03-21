#include <iostream> // TEMP

#include <GLFW/glfw3.h>

#include "OTTJoypad.hpp"

OTTJoypad& OTTJoypad::getInstance(){
	static OTTJoypad instance;
	return instance;
}

OTTJoypad::OTTJoypad() : 
	nGamepads(0),
	gamepads(GLFW_JOYSTICK_LAST + 1, Gamepad()),
	leftStick(0, 1),
	rightStick(3, 4),
	dpad(6, 7),
	fLeftTrigger(0.f),
	fRightTrigger(0.f),
	lastGamepad(0x0),
	parent(0x0)
{
	// Default button input map
	buttonMap[GamepadInput::A]     = 0x0;
	buttonMap[GamepadInput::B]     = 0x1;
	buttonMap[GamepadInput::X]     = 0x2;
	buttonMap[GamepadInput::Y]     = 0x3;
	buttonMap[GamepadInput::LB]    = 0x4;
	buttonMap[GamepadInput::RB]    = 0x5;
	buttonMap[GamepadInput::BACK]  = 0x6;
	buttonMap[GamepadInput::START] = 0x7;
	buttonMap[GamepadInput::HOME]  = 0x8;
	buttonMap[GamepadInput::L3]    = 0x9;
	buttonMap[GamepadInput::R3]    = 0xa;
	buttonMap[GamepadInput::LT]    = 0x2;
	buttonMap[GamepadInput::RT]    = 0x5;
}

void OTTJoypad::enable(){
	setupCallbacks();
	reset();
}

void OTTJoypad::disable(){
	setupCallbacks(false);
}

bool OTTJoypad::poll(const GamepadInput &input){ 
	switch(input){
	case GamepadInput::UP: // Inverted
		return (dpad.fY <= -0.9f);
	case GamepadInput::RIGHT:
		return (dpad.fX >= 0.9f);
	case GamepadInput::DOWN: // Inverted
		return (dpad.fY >= 0.9f);
	case GamepadInput::LEFT:
		return (dpad.fX <= -0.9f);
	case GamepadInput::DPAD:
		return (dpad.fDist >= 0.1f);
	case GamepadInput::LSTICK:
		return (leftStick.fDist >= 0.1f);
	case GamepadInput::RSTICK:
		return (rightStick.fDist >= 0.1f);
	case GamepadInput::LT:
		return (fLeftTrigger >= -0.9f);
	case GamepadInput::RT:
		return (fRightTrigger >= -0.9f);
	default: // Standard button
		return (lastGamepad->nButtonStates[buttonMap[input]] == GLFW_PRESS);
	}
	return false;
}

void OTTJoypad::reset(){
	// Reset gamepad states
	nGamepads = 0;
	for(int i = 0; i <= GLFW_JOYSTICK_LAST; i++){
		if(glfwJoystickPresent(i) == GLFW_TRUE) // Gamepad connected
			connect(i);
		else // Nothing connected
			gamepads[i].disconnect();
	}
}

bool OTTJoypad::status(const int& id) const {
	return (gamepads[id].isConnected() && (glfwJoystickPresent(id) == GLFW_TRUE));
}

bool OTTJoypad::isGamepad(const int& id) const {
#ifdef GLFW_VERSION_3_3
	return (status() && (glfwJoystickIsGamepad(id) == GLFW_TRUE)); // GLFW 3.3
#endif // ifdef GLFW_VERSION_3_3
	return false;
}

bool OTTJoypad::getGamepadState(const int& id) {
#ifdef GLFW_VERSION_3_3
	GLFWgamepadstate state;
	glfwGetGamepadState(nJoystickID, state);
#endif // ifdef GLFW_VERSION_3_3
	return false;
}

void OTTJoypad::update(){
	if(!lastGamepad)
		return;
	lastGamepad->getJoystickButtons();
	lastGamepad->getJoystickHats();
	lastGamepad->getJoystickAxes();
	leftStick.set(lastGamepad->fAxisStates);
	rightStick.set(lastGamepad->fAxisStates);
	dpad.set(lastGamepad->fAxisStates);
	fLeftTrigger = lastGamepad->fAxisStates[2];
	fRightTrigger = lastGamepad->fAxisStates[5];
}


void OTTJoypad::connect(const int& id) {
	if(id > 0 && id <= GLFW_JOYSTICK_LAST){
		gamepads[id].connect(id);
		lastGamepad = &gamepads[id];
		nGamepads++;
	}
}

void OTTJoypad::disconnect(const int& id) {
	if(id > 0 && id <= GLFW_JOYSTICK_LAST){
		gamepads[id].disconnect();
		nGamepads--;
	}
}

void OTTJoypad::print(){
	std::cout << " [joypad] " << nGamepads << " gamepads are connected" << std::endl;
	for(int i = 0; i <= GLFW_JOYSTICK_LAST; i++){
		if(gamepads[i].isConnected())
			gamepads[i].print();
	}
}

void OTTJoypad::joystickCallback(int id, int event) {
	if(event == GLFW_CONNECTED){
		// Joystick connected
		std::cout << " [joypad] Connected! id=" << id << std::endl;
		OTTJoypad::getInstance().connect(id);
	}
	else if(event == GLFW_DISCONNECTED){
		// Joystick disconnected
		std::cout << " [joypad] Disconnected! id=" << id << std::endl;
		OTTJoypad::getInstance().disconnect(id);
	}
}

void OTTJoypad::setupCallbacks(bool bEnable/*=true*/) {
	if(bEnable){
		// Set joystick connected / disconnected callback
		glfwSetJoystickCallback(OTTJoypad::joystickCallback);
	}
	else{
		glfwSetJoystickCallback(0x0); // Unset joystick callback
	}
}

void OTTJoypad::Gamepad::connect(const int& id){
	nID = id;
	bConnected = true;
#ifdef GLFW_VERSION_3_3
	sName = std::string(glfwGetJoystickName(id)); // GLFW 3.3
	jptr = glfwGetJoystickUserPointer(id); // GLFW 3.3
#endif // ifdef GLFW_VERSION_3_3
}

void OTTJoypad::Gamepad::disconnect(){
	bConnected = false;
	nID = -1;
	sName = "";
	jptr = 0x0;
	nButtons = 0;
	nHats = 0;
	nAxes = 0;
	nButtonStates = 0x0;
	nHatStates = 0x0;
	fAxisStates = 0x0;
}

void OTTJoypad::Gamepad::print() const {
	std::cout << " Gamepad id=" << nID << ", name=\"" << sName << "\"" << std::endl;
}

bool OTTJoypad::Gamepad::getJoystickButtons(){
	nButtonStates = glfwGetJoystickButtons(nID, &nButtons); // GLFW 3.0
	return (nButtons > 0);
}

bool OTTJoypad::Gamepad::getJoystickHats(){
#ifdef GLFW_VERSION_3_3
	nHatStates = glfwGetJoystickHats(nID, &nHats); // GLFW 3.3
	return (nHats > 0);
#else
	return false;
#endif // ifdef GLFW_VERSION_3_3
}

bool OTTJoypad::Gamepad::getJoystickAxes(){
	fAxisStates = glfwGetJoystickAxes(nID, &nAxes); // GLFW 3.0
	return (nAxes > 0);
}

