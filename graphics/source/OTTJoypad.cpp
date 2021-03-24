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
	nPlayers(0),
	gamepads(new Gamepad[(GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1) + 1]),
	connected(),
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
	
	// Set GLFW gamepad ID numbers
	for(int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++){
		gamepads[i].setID(i);
	}
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
	connected.clear(); // Remove all existing gamepads
	for(int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++){
		if(glfwJoystickPresent(i) == GLFW_TRUE) // Gamepad connected
			connect(i);
	}
	if(!connected.empty())
		changeActiveGamepad(); // Switch to a different gamepad (if another is connected)
}

bool OTTJoypad::status(const int& id) const {
	return (glfwJoystickPresent(id) == GLFW_TRUE);
}

bool OTTJoypad::isGamepad(const int& id) const {
#ifdef GLFW_VERSION_3_3
	return (status() && (glfwJoystickIsGamepad(id) == GLFW_TRUE)); // GLFW 3.3
#endif // ifdef GLFW_VERSION_3_3
	return false;
}

bool OTTJoypad::getDPadPosition(float& x, float& y) const {
	if(!lastGamepad)
		return false;
	x = lastGamepad->getDPad()->fX;
	y = lastGamepad->getDPad()->fY;
	return true;
}

bool OTTJoypad::getLeftStickPosition(float& x, float& y) const {
	if(!lastGamepad)
		return false;
	x = lastGamepad->getLeftAnalogStick()->fX;
	y = lastGamepad->getLeftAnalogStick()->fY;
	return true;
}

bool OTTJoypad::getLeftStickDeltaPosition(float& dx, float& dy) const {
	if(!lastGamepad)
		return false;
	dx = lastGamepad->getLeftAnalogStick()->fDX;
	dy = lastGamepad->getLeftAnalogStick()->fDY;
	return true;
}

bool OTTJoypad::getRightStickPosition(float& x, float& y) const {
	if(!lastGamepad)
		return false;
	x = lastGamepad->getRightAnalogStick()->fX;
	y = lastGamepad->getRightAnalogStick()->fY;
	return true;
}

bool OTTJoypad::getRightStickDeltaPosition(float& dx, float& dy) const {
	if(!lastGamepad)
		return false;
	dx = lastGamepad->getRightAnalogStick()->fDX;
	dy = lastGamepad->getRightAnalogStick()->fDY;
	return true;
}

float OTTJoypad::getLeftTrigger() const {
	if(lastGamepad)
		return lastGamepad->getLeftTriggerPosition();
	return 0.f;
}

float OTTJoypad::getRightTrigger() const {
	if(lastGamepad)
		return lastGamepad->getRightTriggerPosition();
	return 0.f;
}

void OTTJoypad::update(){
	if(!bConnected || connected.empty())
		return;
	lastGamepad->update();
}

void OTTJoypad::connect(const int& id) {
	if(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST){
		std::cout << " [joypad] Connected gamepad with id=" << id << std::endl;
		if(gamepads[id].connect(id) && gamepads[id].update()){
			connected.push_back(&gamepads[id]);
			connected.back()->setPlayerNumber(nPlayers++);
			nGamepads++;			
			std::cout << " [joypad]  Gamepad assigned player number " << nPlayers - 1 << std::endl;
			if(!lastGamepad){
				lastGamepad = connected.back();
				bConnected = true;
			}
		}
		else{
			std::cout << " [joypad] Warning! Connected device at id=" << id << " is not a valid gamepad.\n";
		}
	}
}

void OTTJoypad::disconnect(const int& id) {
	if(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST){
		auto pad = findGamepad(id);
		if(pad != connected.end()){ // Disconnected gamepad is in the list of valid gamepads
			std::cout << " [joypad] Disconnected gamepad with id=" << id << std::endl;
			connected.erase(pad);
			nGamepads--;
			if((*pad) == lastGamepad){ // User disconnected the currently active controller
				lastGamepad = 0x0;
				bConnected = false;
				if(!connected.empty())
					changeActiveGamepad(); // Switch to a different gamepad (if another is connected)
			}
		}
		gamepads[id].disconnect();
	}
}

bool OTTJoypad::changeActiveGamepad(const int& id/*=-1*/){
	if(connected.empty()){
		std::cout << " [joypad] changeActiveGamepad() Error! No gamepads currently connected." << std::endl;
		return false;
	}
	GamepadIterator pad;
	if(id < 0){ // Next gamepad in the list
		if(lastGamepad){ // Advance to the next available gamepad
			pad = findGamepad(lastGamepad->getID()) + 1;
			if(pad == connected.end()) // Return to first gamepad
				pad = connected.begin();
		}
		else{ // No active gamepads, return to beginning of list
			pad = connected.begin();
		}
	}
	else{ // User specified gamepad
		if(id < GLFW_JOYSTICK_1 || id > GLFW_JOYSTICK_LAST){
			std::cout << " [joypad] changeActiveGamepad() Error! Invalid gamepad ID (" << id << ")." << std::endl;
			return false;
		}
		pad = findGamepad(id);
	}
	if(pad == connected.end()){
		std::cout << " [joypad] changeActiveGamepad() Error! Specified gamepad ID (" << id << ") is not connected." << std::endl;
		return false;
	}
	lastGamepad = (*pad);
	bConnected = true;
	std::cout << " [joypad] Switched to gamepad " << lastGamepad->getID() << " (player " << lastGamepad->getPlayerNumber() << ")" << std::endl;
	return true;
}

bool OTTJoypad::changeActivePlayer(const int& player){
	GamepadIterator pad = findPlayer(player); 
	if(pad == connected.end()){
		std::cout << " [joypad] changeActivePlayer() Error! Specified player ID (" << player << ") does not exist." << std::endl;
		return false;
	}
	lastGamepad = (*pad);
	bConnected = true;
	std::cout << " [joypad] Switched to gamepad " << lastGamepad->getID() << " (player " << lastGamepad->getPlayerNumber() << ")" << std::endl;
	return true;
}

void OTTJoypad::print(){
	std::cout << " [joypad] " << connected.size() << " gamepads are connected" << std::endl;
	for(auto pad = connected.cbegin(); pad != connected.cend(); pad++){
		(*pad)->print();
	}
}

void OTTJoypad::joystickCallback(int id, int event) {
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

void OTTJoypad::reorderPlayers(){
	if(connected.empty())
		return;

	// Sort connected gamepads by player number
	std::sort(
		connected.begin(), 
		connected.end(), 
		[](Gamepad* left, Gamepad* right) -> bool 
		{ 
			return (left->getPlayerNumber() > right->getPlayerNumber()); 
		} 
	);

	// Remove any gaps in player number
	int nextPlayer = 0;
	for(auto pad = connected.cbegin(); pad != connected.cend(); pad++){
		if((*pad)->getPlayerNumber() != nextPlayer)
			(*pad)->setPlayerNumber(nextPlayer);
		nextPlayer++;
	}
}

GamepadIterator OTTJoypad::findGamepad(const int& id){
	for(GamepadIterator pad = connected.begin(); pad != connected.end(); pad++){
		if((*pad)->getID() == id)
			return pad;
	}
	return connected.end();
}

GamepadIterator OTTJoypad::findPlayer(const int& player){
	for(GamepadIterator pad = connected.begin(); pad != connected.end(); pad++){
		if((*pad)->getPlayerNumber() == player)
			return pad;
	}
	return connected.end();
}

bool Gamepad::connect(const int& id){
	nID = id;
	bConnected = true;
#ifdef GLFW_VERSION_3_3
	sName = std::string(glfwGetJoystickName(id)); // GLFW 3.3
	jptr = glfwGetJoystickUserPointer(id); // GLFW 3.3
#endif // ifdef GLFW_VERSION_3_3
	return true;
}

void Gamepad::disconnect(){
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
	for(int i = 0; i < 15; i++)
		bToggleStates[i] = false;
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

bool Gamepad::update(){
#ifdef GLFW_VERSION_3_3
	glfwGetGamepadState(nID, buttonStates); // GLFW 3.3
#else
	if(!bValidGamepad)
		return false;
	if(!getJoystickButtons()){
		bValidGamepad = false;
		return false;
	}
	getJoystickHats();
	getJoystickAxes();
#endif // ifdef GLFW_VERSION_3_3
	this->updateButtonData();
	return true;
}

void Gamepad::print() const {
	std::cout << "  Gamepad id=" << nID << ", name=\"" << sName << "\"" << std::endl;
}

bool Gamepad::getJoystickButtons(){
	nButtonStates = glfwGetJoystickButtons(nID, &nButtons); // GLFW 3.0
	return (nButtons > 0);
}

bool Gamepad::getJoystickHats(){
#ifdef GLFW_VERSION_3_3
	nHatStates = glfwGetJoystickHats(nID, &nHats); // GLFW 3.3
	return (nHats > 0);
#else
	return false;
#endif // ifdef GLFW_VERSION_3_3
}

bool Gamepad::getJoystickAxes(){
	fAxisStates = glfwGetJoystickAxes(nID, &nAxes); // GLFW 3.0
	return (nAxes > 0);
}

void Gamepad::updateButtonData(){
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

