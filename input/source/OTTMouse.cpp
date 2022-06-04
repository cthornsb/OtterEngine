#include "OTTMouse.hpp"

#include <graphics/core/OTTWindow.hpp>

ott::Mouse& ott::Mouse::Instance() {
	static Mouse instance;
	return instance;
}

ott::Mouse::Mouse() :
	state(MouseStates::Normal),
	nCount(0),
	dPosX(0),
	dPosY(0),
	dDeltaX(0),
	dDeltaY(0),
	bStates{ false, false, false },
	bPressed{ false, false, false },
	bReleased{ false, false, false },
	cursor(0x0),
	parent(0x0)
{
}

void ott::Mouse::Enable(){
	this->SetupCallbacks();
}

void ott::Mouse::Disable(){
	this->SetupCallbacks(false);
}

void ott::Mouse::NormalCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	state = MouseStates::Normal;
}

void ott::Mouse::DisableCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	state = MouseStates::Disabled;
}

void ott::Mouse::HideCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	state = MouseStates::Hidden;
}

bool ott::Mouse::RawCursor(){
#ifdef GLFW_RAW_MOUSE_MOTION
	if (glfwRawMouseMotionSupported()){
		this->DisableCursor(); // Disable normal cursor
		glfwSetInputMode(parent, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		state = MouseStates::Raw;
		return true;
	}
#endif // ifdef GLFW_RAW_MOUSE_MOTION
	return false;
}

bool ott::Mouse::SetCursorState(const MouseStates& newState){
	switch(newState){
	case MouseStates::Normal:
		this->NormalCursor();
		return true;
	case MouseStates::Disabled:
		this->DisableCursor();
		return true;
	case MouseStates::Hidden:
		this->HideCursor();
		return true;
	case MouseStates::Raw:
		return this->RawCursor();
	default:
		break;
	}
	return false;
}

GLFWcursor* ott::Mouse::SetCustomCursor(uint8_t* pixels, const int32_t& w, const int32_t& h, const int32_t& x0, const int32_t& y0){
	GLFWimage image;
	image.width = w;
	image.height = h;
	image.pixels = pixels;

	cursor = glfwCreateCursor(&image, x0, y0);
	glfwSetCursor(parent, cursor);
	
	return cursor;
}

void ott::Mouse::RemoveCustomCursor(){
	glfwSetCursor(parent, NULL); // disable custom cursor
	glfwDestroyCursor(cursor);
}

bool ott::Mouse::Delta() {
	return (dDeltaX != 0 || dDeltaY != 0);
}

void ott::Mouse::SetPosition(const double& x, const double& y) {
	dDeltaX = x - dPosX;
	dDeltaY = y - dPosY;
	dPosX = x;
	dPosY = y;
}

bool ott::Mouse::Delta(double& deltaX, double& deltaY) {
	deltaX = dDeltaX;
	deltaY = dDeltaY;
	dDeltaX = 0;
	dDeltaY = 0;
	return (deltaX != 0 || deltaY != 0);
}

bool ott::Mouse::Poll(const int32_t& button) {
	if (bPressed[button]) {
		bPressed[button] = false;
		return true;
	}
	return false;
}

bool ott::Mouse::Pressed(const int32_t& button) {
	return this->Poll(button);
}

bool ott::Mouse::Released(const int32_t& button) {
	if (bReleased[button]) {
		bReleased[button] = false;
		return true;
	}
	return false;
}

void ott::Mouse::Down(const int32_t& button) {
	if (!bStates[button]) {
		bStates[button] = true;
		bPressed[button] = true;
		bReleased[button] = false;
		nCount++;
	}
}

void ott::Mouse::Up(const int32_t& button) {
	if (bStates[button]) {
		bStates[button] = false;
		bPressed[button] = false;
		bReleased[button] = true;
		nCount--;
	}
}

void ott::Mouse::Reset() {
	for (int32_t i = 0; i < 3; i++) {
		bStates[i] = false;
		bPressed[i] = false;
		bReleased[i] = false;
	}
	nCount = 0;
}

void ott::Mouse::HandleMouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods){
    //if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
     //   popup_menu(); GLFW_MOUSE_BUTTON_LEFT
	Window* currentWindow = ActiveWindows::Instance().Find(window);
	if (action == GLFW_RELEASE) {
		currentWindow->GetMouse()->Up(button);
	}
	else if (action == GLFW_PRESS) {
		currentWindow->GetMouse()->Down(button);
	}
}

void ott::Mouse::HandleMouseMove(GLFWwindow* window, double x, double y){
	// Handle active mouse movement (i.e. movement while holding a mouse button)
	Window* currentWindow = ActiveWindows::Instance().Find(window);
	currentWindow->GetMouse()->SetPosition(x, y);
}

void ott::Mouse::HandleMouseEnter(GLFWwindow* window, int32_t entered){
	if (entered){ // Cursor entered the window
	}
	else{ // Cursor left the window
	}
}

void ott::Mouse::HandleMouseScroll(GLFWwindow* window, double xoffset, double yoffset){
}

void ott::Mouse::SetupCallbacks(bool bEnable/*=true*/) {
	if(bEnable){
		glfwSetMouseButtonCallback(parent, ott::Mouse::HandleMouse); // Set mouse button callback
		glfwSetCursorPosCallback(parent, ott::Mouse::HandleMouseMove); // Set mouse movement callback
		glfwSetCursorEnterCallback(parent, ott::Mouse::HandleMouseEnter); // Set cursor enter / exit callback
		glfwSetScrollCallback(parent, ott::Mouse::HandleMouseScroll); // Set mouse scroll callback
	}
	else{
		glfwSetMouseButtonCallback(parent, 0x0); // Unset mouse button callback
		glfwSetCursorPosCallback(parent, 0x0); // Unset mouse movement callback
		glfwSetCursorEnterCallback(parent, 0x0); // Unset curser enter / exit callback
		glfwSetScrollCallback(parent, 0x0); // Unset mouse scroll callback	
	}
}

