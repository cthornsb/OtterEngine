#include <GLFW/glfw3.h>

#include "OTTJoypad.hpp"
#include "OTTGamepad.hpp"

#include <core/OTTFrameTimer.hpp>

#include <algorithm>
#include <iostream>

ott::Joypad& ott::Joypad::Instance(){
	static Joypad instance;
	return instance;
}

ott::Joypad::Joypad() : 
	bConnected(false),
	nGamepads(0),
	nPlayers(0),
	fDPadThreshold(0.05f),
	fLeftStickThreshold(0.05f),
	fRightStickThreshold(0.05f),
	fLeftTriggerThreshold(0.95f),
	fRightTriggerThreshold(0.95f),
	gamepads(new Gamepad[(GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1) + 1]),
	connected(),
	buttonMap(),
	lastGamepad(0x0),
	parent(0x0)
{
	// Default button input map (360-like)
	buttonMap[GamepadInput::A]     = std::make_pair(GLFW_GAMEPAD_BUTTON_A, "A");
	buttonMap[GamepadInput::B]     = std::make_pair(GLFW_GAMEPAD_BUTTON_B, "B");
	buttonMap[GamepadInput::X]     = std::make_pair(GLFW_GAMEPAD_BUTTON_X, "X");
	buttonMap[GamepadInput::Y]     = std::make_pair(GLFW_GAMEPAD_BUTTON_Y, "Y");
	buttonMap[GamepadInput::LB]    = std::make_pair(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, "LB");
	buttonMap[GamepadInput::RB]    = std::make_pair(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, "RB");
	buttonMap[GamepadInput::Back]  = std::make_pair(GLFW_GAMEPAD_BUTTON_BACK, "back");
	buttonMap[GamepadInput::Start] = std::make_pair(GLFW_GAMEPAD_BUTTON_START, "start");
	buttonMap[GamepadInput::Guide] = std::make_pair(GLFW_GAMEPAD_BUTTON_GUIDE, "guide");
	buttonMap[GamepadInput::L3]    = std::make_pair(GLFW_GAMEPAD_BUTTON_LEFT_THUMB, "L3");
	buttonMap[GamepadInput::R3]    = std::make_pair(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, "R3");
	buttonMap[GamepadInput::Up]    = std::make_pair(GLFW_GAMEPAD_BUTTON_DPAD_UP, "up");
	buttonMap[GamepadInput::Right] = std::make_pair(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, "right");
	buttonMap[GamepadInput::Down]  = std::make_pair(GLFW_GAMEPAD_BUTTON_DPAD_DOWN, "down");
	buttonMap[GamepadInput::Left]  = std::make_pair(GLFW_GAMEPAD_BUTTON_DPAD_LEFT, "left");

	// Face button aliases (PS-like)
	buttonMap[GamepadInput::Cross]    = std::make_pair(GLFW_GAMEPAD_BUTTON_A, "cross");
	buttonMap[GamepadInput::Circle]   = std::make_pair(GLFW_GAMEPAD_BUTTON_B, "circle");
	buttonMap[GamepadInput::Square]   = std::make_pair(GLFW_GAMEPAD_BUTTON_X, "square");
	buttonMap[GamepadInput::Triangle] = std::make_pair(GLFW_GAMEPAD_BUTTON_Y, "triangle");
	
	// Set GLFW gamepad ID numbers
	for(int32_t i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++){
		gamepads[i].SetId(i);
	}
}

void ott::Joypad::Enable(){
	this->SetupCallbacks();
	this->Reset();
}

void ott::Joypad::Disable(){
	this->SetupCallbacks(false);
}

bool ott::Joypad::Connected() const {
	return (lastGamepad && lastGamepad->Connected());
}

/** Return true if a gamepad is connected and button data is available for use
  */
bool ott::Joypad::Ready() const {
	return (lastGamepad && lastGamepad->Good());
}

bool ott::Joypad::Poll(const GamepadInput &input){ 
	if (!lastGamepad || lastGamepad->Good() == false) {
		return false;
	}
	return (lastGamepad->Poll(buttonMap[input].first));
}

bool ott::Joypad::Check(const GamepadInput &input){ 
	if(!lastGamepad || !lastGamepad->Good())
		return false;
	switch(input){
#ifndef GLFW_VERSION_3_3
	// Older versions of GLFW treat the DPad as an analog stick, so we need to 
	// convert the floating point DPad position into a binary button state.
	case GamepadInput::Up: // Inverted
		return (lastGamepad->DPad()->fY <= -(1.f - fDPadThreshold));
	case GamepadInput::Right:
		return (lastGamepad->DPad()->fX >= (1.f - fDPadThreshold));
	case GamepadInput::Down: // Inverted
		return (lastGamepad->DPad()->fY >= (1.f - fDPadThreshold));
	case GamepadInput::Left:
		return (lastGamepad->DPad()->fX <= -(1.f - fDPadThreshold));
#endif // ifndef GLFW_VERSION_3_3
	case GamepadInput::DPad:
		return (lastGamepad->DPad()->fDist >= fDPadThreshold);
	case GamepadInput::LStick:
		return (lastGamepad->LeftAnalogStick()->fDist >= fLeftStickThreshold);
	case GamepadInput::RStick:
		return (lastGamepad->RightAnalogStick()->fDist >= fRightStickThreshold);
	case GamepadInput::LT:
		return (lastGamepad->LeftTriggerPosition() >= -fLeftTriggerThreshold);
	case GamepadInput::RT:
		return (lastGamepad->RightTriggerPosition() >= -fRightTriggerThreshold);
	default: // Standard button
		return (lastGamepad->Check(buttonMap[input].first));
	}
	return false;
}

void ott::Joypad::Reset(){
	// Reset gamepad states
	nGamepads = 0;
	lastGamepad = 0x0;
	bConnected = false;
	connected.clear(); // Remove all existing gamepads
	for(int32_t i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++){
		if (glfwJoystickPresent(i) == GLFW_TRUE) { // Gamepad connected
			this->Connect(i);
		}
	}
	if (connected.empty() == false) {
		this->ChangeActiveGamepad(); // Switch to a different gamepad (if another is connected)
	}
}

bool ott::Joypad::Status(const int32_t& id) const {
	return (glfwJoystickPresent(id) == GLFW_TRUE);
}

bool ott::Joypad::IsGamepad(const int32_t& id) const {
#ifdef GLFW_VERSION_3_3
	return (status(id) && (glfwJoystickIsGamepad(id) == GLFW_TRUE)); // GLFW 3.3
#endif // ifdef GLFW_VERSION_3_3
	return false;
}

bool ott::Joypad::DPadPosition(float& x, float& y) const {
	if(!lastGamepad)
		return false;
	x = lastGamepad->DPad()->fX;
	y = lastGamepad->DPad()->fY;
	return true;
}

bool ott::Joypad::LeftStickPosition(float& x, float& y) const {
	if(!lastGamepad)
		return false;
	x = lastGamepad->LeftAnalogStick()->fX;
	y = lastGamepad->LeftAnalogStick()->fY;
	return true;
}

bool ott::Joypad::LeftStickDeltaPosition(float& dx, float& dy) const {
	if(!lastGamepad)
		return false;
	dx = lastGamepad->LeftAnalogStick()->fDX;
	dy = lastGamepad->LeftAnalogStick()->fDY;
	return true;
}

bool ott::Joypad::RightStickPosition(float& x, float& y) const {
	if(!lastGamepad)
		return false;
	x = lastGamepad->RightAnalogStick()->fX;
	y = lastGamepad->RightAnalogStick()->fY;
	return true;
}

bool ott::Joypad::RightStickDeltaPosition(float& dx, float& dy) const {
	if(!lastGamepad)
		return false;
	dx = lastGamepad->RightAnalogStick()->fDX;
	dy = lastGamepad->RightAnalogStick()->fDY;
	return true;
}

float ott::Joypad::LeftTrigger() const {
	if(lastGamepad)
		return lastGamepad->LeftTriggerPosition();
	return 0.f;
}

float ott::Joypad::RightTrigger() const {
	if(lastGamepad)
		return lastGamepad->RightTriggerPosition();
	return 0.f;
}

bool ott::Joypad::Calibrate(const int32_t& cycles/*=10*/) {
	if (!lastGamepad || cycles <= 0)
		return false;
	this->Calibrate(lastGamepad);
	return true;
}

void ott::Joypad::Update(){
	if(!bConnected || connected.empty())
		return;
	lastGamepad->Update();
}

void ott::Joypad::Connect(const int32_t& id) {
	if(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST){
		auto pad = this->FindGamepad(id);
		if (pad != connected.end()) { // Gamepad is already connected, nothing to do
			return;
		}
		std::cout << " [joypad] Connected gamepad with id=" << id << std::endl;
		if(gamepads[id].Connect(id) && gamepads[id].Update()){
			connected.push_back(&gamepads[id]);
			connected.back()->SetPlayerNumber(nPlayers++);
			nGamepads++;			
			std::cout << " [joypad]  Gamepad assigned player number " << nPlayers - 1 << std::endl;
			if(!lastGamepad){
				lastGamepad = connected.back();
				bConnected = true;
			}
			this->Calibrate(connected.back()); // Calibrate new gamepad
		}
		else{
			std::cout << " [joypad] Warning! Connected device at id=" << id << " is not a valid gamepad.\n";
		}
	}
}

void ott::Joypad::Disconnect(const int32_t& id) {
	if(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST){
		auto pad = this->FindGamepad(id);
		if(pad != connected.end()){ // Disconnected gamepad is in the list of valid gamepads
			std::cout << " [joypad] Disconnected gamepad with id=" << id << std::endl;
			connected.erase(pad);
			nGamepads--;
			if((*pad) == lastGamepad){ // User disconnected the currently active controller
				lastGamepad = 0x0;
				bConnected = false;
				if (!connected.empty()) {
					this->ChangeActiveGamepad(); // Switch to a different gamepad (if another is connected)
				}
			}
		}
		gamepads[id].Disconnect();
	}
}

bool ott::Joypad::ChangeActiveGamepad(const int32_t& id/*=-1*/){
	if(connected.empty()){
		std::cout << " [joypad] changeActiveGamepad() Error! No gamepads currently connected." << std::endl;
		return false;
	}
	GamepadIterator pad;
	if(id < 0){ // Next gamepad in the list
		if(lastGamepad){ // Advance to the next available gamepad
			pad = this->FindGamepad(lastGamepad->Id()) + 1;
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
		pad = this->FindGamepad(id);
	}
	if(pad == connected.end()){
		std::cout << " [joypad] changeActiveGamepad() Error! Specified gamepad ID (" << id << ") is not connected." << std::endl;
		return false;
	}
	lastGamepad = (*pad);
	bConnected = true;
	std::cout << " [joypad] Switched to gamepad " << lastGamepad->Id() << " (player " << lastGamepad->PlayerNumber() << ")" << std::endl;
	return true;
}

bool ott::Joypad::ChangeActivePlayer(const int32_t& player){
	GamepadIterator pad = this->FindPlayer(player); 
	if(pad == connected.end()){
		std::cout << " [joypad] changeActivePlayer() Error! Specified player ID (" << player << ") does not exist." << std::endl;
		return false;
	}
	lastGamepad = (*pad);
	bConnected = true;
	std::cout << " [joypad] Switched to gamepad " << lastGamepad->Id() << " (player " << lastGamepad->PlayerNumber() << ")" << std::endl;
	return true;
}

void ott::Joypad::Print(){
	std::cout << " [joypad] " << connected.size() << " gamepads are connected" << std::endl;
	for(auto pad = connected.cbegin(); pad != connected.cend(); pad++){
		(*pad)->Print();
	}
}

bool ott::Joypad::FindNamedButton(const std::string& name, GamepadInput& button) const {
	for(auto btn = buttonMap.cbegin(); btn != buttonMap.cend(); btn++){
		if(btn->second.second == name){
			button = btn->first;
			return true;
		}
	}
	return false;
}

void ott::Joypad::JoystickCallback(int32_t id, int32_t event) {
	if(event == GLFW_CONNECTED){
		// Joystick connected
		ott::Joypad::Instance().Connect(id);
	}
	else if(event == GLFW_DISCONNECTED){
		// Joystick disconnected
		ott::Joypad::Instance().Disconnect(id);
	}
}

void ott::Joypad::SetupCallbacks(bool bEnable/*=true*/) {
	if(bEnable){
		// Set joystick connected / disconnected callback
		glfwSetJoystickCallback(ott::Joypad::JoystickCallback);
	}
	else{
		glfwSetJoystickCallback(0x0); // Unset joystick callback
	}
}

void ott::Joypad::ReorderPlayers(){
	if (connected.empty() == true) {
		return;
	}

	// Sort connected gamepads by player number
	std::sort(
		connected.begin(), 
		connected.end(), 
		[](Gamepad* left, Gamepad* right) -> bool 
		{ 
			return (left->PlayerNumber() > right->PlayerNumber()); 
		} 
	);

	// Remove any gaps in player number
	int32_t nextPlayer = 0;
	for(auto pad = connected.cbegin(); pad != connected.cend(); pad++){
		if((*pad)->PlayerNumber() != nextPlayer)
			(*pad)->SetPlayerNumber(nextPlayer);
		nextPlayer++;
	}
}

ott::GamepadIterator ott::Joypad::FindGamepad(const int32_t& id){
	for(GamepadIterator pad = connected.begin(); pad != connected.end(); pad++){
		if((*pad)->Id() == id)
			return pad;
	}
	return connected.end();
}

ott::GamepadIterator ott::Joypad::FindPlayer(const int32_t& player){
	for(GamepadIterator pad = connected.begin(); pad != connected.end(); pad++){
		if((*pad)->PlayerNumber() == player)
			return pad;
	}
	return connected.end();
}

void ott::Joypad::Calibrate(Gamepad* pad, const int32_t& cycles/*=10*/) {
	if (!pad || cycles <= 0)
		return;
	float avgLX = 0.f, avgLY = 0.f;
	float avgRX = 0.f, avgRY = 0.f;
	for (int32_t i = 0; i < cycles; i++) {
		pad->Update();
		avgLX += pad->LeftAnalogStick()->fX;
		avgLY += pad->LeftAnalogStick()->fY;
		avgRX += pad->RightAnalogStick()->fX;
		avgRY += pad->RightAnalogStick()->fY;
		FrameTimer::Sleep(1000); // Sleep for a short time
	}
	avgLX /= cycles;
	avgLY /= cycles;
	avgRX /= cycles;
	avgRY /= cycles;
	pad->LeftAnalogStick()->SetCalibration(avgLX, avgLY);
	pad->RightAnalogStick()->SetCalibration(avgRX, avgRY);
	std::cout << " [joypad] Calibrate: id=" << pad->Id() << ", left=(" << avgLX << ", " << avgLY << "), right=(" << avgRX << ", " << avgRY << ")" << std::endl;
}



