#include <iostream> // TEMP
#include <algorithm> // find
#include <cstring> // memcpy

#include <GLFW/glfw3.h>

#include "OTTJoypad.hpp"

OTTJoypad& OTTJoypad::getInstance(){
	static OTTJoypad instance;
	return instance;
}

OTTJoypad::OTTJoypad() : 
	bConnected(false),
	nGamepads(0),
	gamepads(),
	buttonMap(),
	lastGamepad(0x0),
	parent(0x0)
{
	// Default button input map (360-like)
	buttonMap[GamepadInput::A]     = GLFW_GAMEPAD_BUTTON_A;
	buttonMap[GamepadInput::B]     = GLFW_GAMEPAD_BUTTON_B;
	buttonMap[GamepadInput::X]     = GLFW_GAMEPAD_BUTTON_X;
	buttonMap[GamepadInput::Y]     = GLFW_GAMEPAD_BUTTON_Y;
	buttonMap[GamepadInput::LB]    = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
	buttonMap[GamepadInput::RB]    = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
	buttonMap[GamepadInput::BACK]  = GLFW_GAMEPAD_BUTTON_BACK;
	buttonMap[GamepadInput::START] = GLFW_GAMEPAD_BUTTON_START;
	buttonMap[GamepadInput::GUIDE] = GLFW_GAMEPAD_BUTTON_GUIDE;
	buttonMap[GamepadInput::L3]    = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
	buttonMap[GamepadInput::R3]    = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
	buttonMap[GamepadInput::UP]    = GLFW_GAMEPAD_BUTTON_DPAD_UP;
	buttonMap[GamepadInput::RIGHT] = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
	buttonMap[GamepadInput::DOWN]  = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
	buttonMap[GamepadInput::LEFT]  = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;

	// Face button aliases (PS-like)
	buttonMap[GamepadInput::CROSS]    = GLFW_GAMEPAD_BUTTON_A;
	buttonMap[GamepadInput::CIRCLE]   = GLFW_GAMEPAD_BUTTON_B;
	buttonMap[GamepadInput::SQUARE]   = GLFW_GAMEPAD_BUTTON_X;
	buttonMap[GamepadInput::TRIANGLE] = GLFW_GAMEPAD_BUTTON_Y;
}

void OTTJoypad::enable(){
	setupCallbacks();
	reset();
}

void OTTJoypad::disable(){
	setupCallbacks(false);
}

bool OTTJoypad::poll(const GamepadInput &input){ 
	if(!lastGamepad || !lastGamepad->isGood())
		return false;
	return (lastGamepad->poll(buttonMap[input]));
}

bool OTTJoypad::check(const GamepadInput &input){ 
	if(!lastGamepad || !lastGamepad->isGood())
		return false;
	switch(input){
#ifndef GLFW_VERSION_3_3
	// Older versions of GLFW treat the DPad as an analog stick, so we need to 
	// convert the floating point DPad position into a binary button state.
	case GamepadInput::UP: // Inverted
		return (lastGamepad->getDPad()->fY <= -0.9f);
	case GamepadInput::RIGHT:
		return (lastGamepad->getDPad()->fX >= 0.9f);
	case GamepadInput::DOWN: // Inverted
		return (lastGamepad->getDPad()->fY >= 0.9f);
	case GamepadInput::LEFT:
		return (lastGamepad->getDPad()->fX <= -0.9f);
#endif // ifndef GLFW_VERSION_3_3
	case GamepadInput::DPAD:
		return (lastGamepad->getDPad()->fDist >= 0.1f);
	case GamepadInput::LSTICK:
		return (lastGamepad->getLeftAnalogStick()->fDist >= 0.1f);
	case GamepadInput::RSTICK:
		return (lastGamepad->getRightAnalogStick()->fDist >= 0.1f);
	case GamepadInput::LT:
		return (lastGamepad->getLeftTriggerPosition() >= -0.9f);
	case GamepadInput::RT:
		return (lastGamepad->getRightTriggerPosition() >= -0.9f);
	default: // Standard button
		return (lastGamepad->check(buttonMap[input]));
	}
	return false;
}

void OTTJoypad::reset(){
	// Reset gamepad states
	nGamepads = 0;
	lastGamepad = 0x0;
	bConnected = false;
	gamepads.clear(); // Remove all existing gamepads
	for(int i = 0; i <= GLFW_JOYSTICK_LAST; i++){
		if(glfwJoystickPresent(i) == GLFW_TRUE) // Gamepad connected
			connect(i);
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

void OTTJoypad::update(){
	if(!bConnected || gamepads.empty())
		return;
	lastGamepad->update();
}

void OTTJoypad::connect(const int& id) {
	if(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST){
		std::cout << " [joypad] Connected gamepad with id=" << id << std::endl;
		gamepads.push_back(Gamepad(id));
		gamepads.back().connect(id);
		lastGamepad = &gamepads.back();
		bConnected = true;
		nGamepads++;
	}
}

void OTTJoypad::disconnect(const int& id) {
	if(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST){
		auto pad = findGamepad(id);
		if(pad != gamepads.end()){
			std::cout << " [joypad] Disconnected gamepad with id=" << id << std::endl;
			if(&(*pad) == lastGamepad){ // User disconnected the currently active controller
				lastGamepad = 0x0;
				bConnected = false;
			}
			gamepads.erase(pad);
			nGamepads--;
		}
		else{
			std::cout << " [joypad] Error! Failed to find gamepad with id=" << id << std::endl;
		}
	}
}

bool OTTJoypad::changeActiveGamepad(const int& id/*=-1*/){
	if(gamepads.size() < 2){
		std::cout << " [joypad] Error! " << gamepads.size() << " gamepads currently connected." << std::endl;
		return false;
	}
	std::vector<Gamepad>::iterator pad;
	if(id < 0){ // Next gamepad in the list
		pad = findGamepad(lastGamepad);
		pad++;
		if(pad == gamepads.end()) // Return to first gamepad
			pad = gamepads.begin();
	}
	else{ // User specified gamepad
		if(id < GLFW_JOYSTICK_1 || id > GLFW_JOYSTICK_LAST){
			std::cout << " [joypad] Error! Invalid gamepad ID (" << id << ")." << std::endl;
			return false;
		}
		pad = findGamepad(id);
	}
	if(pad == gamepads.end()){
		std::cout << " [joypad] Error! Specified gamepad ID (" << id << ") is not connected." << std::endl;
		return false;
	}
	lastGamepad = &(*pad);
	bConnected = true;
	std::cout << " [joypad] Switched to gamepad " << lastGamepad->getID() << std::endl;
	return true;
}

void OTTJoypad::print(){
	std::cout << " [joypad] " << gamepads.size() << " gamepads are connected" << std::endl;
	for(auto pad = gamepads.cbegin(); pad != gamepads.cend(); pad++){
		pad->print();
	}
}

void OTTJoypad::joystickCallback(int id, int event) {
	std::cout << " CALLBACK id=" << id << std::endl;
	if(event == GLFW_CONNECTED){
		// Joystick connected
		OTTJoypad::getInstance().connect(id);
	}
	else if(event == GLFW_DISCONNECTED){
		// Joystick disconnected
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

std::vector<Gamepad>::iterator OTTJoypad::findGamepad(const int& id){
	return std::find(gamepads.begin(), gamepads.end(), id);
}

std::vector<Gamepad>::iterator OTTJoypad::findGamepad(LogicalGamepad* id){
	return std::find(gamepads.begin(), gamepads.end(), id);
}

void LogicalGamepad::connect(const int& id){
	nID = id;
	bConnected = true;
#ifdef GLFW_VERSION_3_3
	sName = std::string(glfwGetJoystickName(id)); // GLFW 3.3
	jptr = glfwGetJoystickUserPointer(id); // GLFW 3.3
#endif // ifdef GLFW_VERSION_3_3
}

void LogicalGamepad::disconnect(){
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

void LogicalGamepad::print() const {
	std::cout << " Gamepad id=" << nID << ", name=\"" << sName << "\"" << std::endl;
}

void LogicalGamepad::update(){
#ifdef GLFW_VERSION_3_3
	glfwGetGamepadState(nID, buttonStates); // GLFW 3.3
#else
	getJoystickButtons();
	getJoystickHats();
	getJoystickAxes();
#endif // ifdef GLFW_VERSION_3_3
	this->onUserUpdate();
}

bool LogicalGamepad::getJoystickButtons(){
	nButtonStates = glfwGetJoystickButtons(nID, &nButtons); // GLFW 3.0
	return (nButtons > 0);
}

bool LogicalGamepad::getJoystickHats(){
#ifdef GLFW_VERSION_3_3
	nHatStates = glfwGetJoystickHats(nID, &nHats); // GLFW 3.3
	return (nHats > 0);
#else
	return false;
#endif // ifdef GLFW_VERSION_3_3
}

bool LogicalGamepad::getJoystickAxes(){
	fAxisStates = glfwGetJoystickAxes(nID, &nAxes); // GLFW 3.0
	return (nAxes > 0);
}

Gamepad::Gamepad(const int& id) :
	LogicalGamepad(id),
	bGood(false),
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

void Gamepad::onUserUpdate(){
#ifdef GLFW_VERSION_3_3
	leftStick.set(
		buttonStates.axes[GLFW_GAMEPAD_AXIS_LEFT_X],
		buttonStates.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]
	);
	rightStick.set(
		buttonStates.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
		buttonStates.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]
	);
	fLeftTrigger = buttonStates.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
	fRightTrigger = buttonStates.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
	for(int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++){
		if(buttonStates.buttons[i] != prevStates.buttons[i]){ // Button changed states
			if(buttonStates.buttons[i] == GLFW_PRESS) // Button pressed
				bToggleStates[i] = true;
			else // Button released
				bToggleStates[i] = false;
		}
		prevStates.buttons[i] = buttonStates.buttons[i];
	}
#else
	leftStick.set(fAxisStates);
	rightStick.set(fAxisStates);
	dpad.set(fAxisStates);
	fLeftTrigger = fAxisStates[2];
	fRightTrigger = fAxisStates[5];
	for(int i = 0; i < nButtons; i++){
		if(nButtonStates[i] != prevStates.buttons[i]){ // Button changed states
			if(nButtonStates[i] == GLFW_PRESS) // Button pressed
				bToggleStates[i] = true;
			else // Button released
				bToggleStates[i] = false;
		}
		prevStates.buttons[i] = nButtonStates[i];
	}
#endif // ifdef GLFW_VERSION_3_3
	bGood = true;
}
 
bool Gamepad::poll(const unsigned char& input){
	if(bToggleStates[input]){
		bToggleStates[input] = false;
		return true;
	}
	return false;
}

bool Gamepad::check(const unsigned char& input){
	return bToggleStates[input];
}

