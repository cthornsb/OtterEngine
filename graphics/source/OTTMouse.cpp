#include "OTTMouse.hpp"
#include "OTTWindow.hpp"

OTTMouse::OTTMouse() :
	state(MouseStates::NORMAL),
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

void OTTMouse::enable(){
	setupCallbacks();
}

void OTTMouse::disable(){
	setupCallbacks(false);
}

void OTTMouse::setNormalCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	state = MouseStates::NORMAL;
}

void OTTMouse::setDisableCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	state = MouseStates::DISABLED;
}

void OTTMouse::setHideCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	state = MouseStates::HIDDEN;
}

bool OTTMouse::setRawCursor(){
#ifdef GLFW_RAW_MOUSE_MOTION
	if (glfwRawMouseMotionSupported()){
		setDisableCursor(); // Disable normal cursor
		glfwSetInputMode(parent, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		state = MouseStates::RAW;
		return true;
	}
#endif // ifdef GLFW_RAW_MOUSE_MOTION
	return false;
}

bool OTTMouse::setCursorState(const MouseStates& newState){
	switch(newState){
	case MouseStates::NORMAL:
		setNormalCursor();
		return true;
	case MouseStates::DISABLED:
		setDisableCursor();
		return true;
	case MouseStates::HIDDEN:
		setHideCursor();
		return true;
	case MouseStates::RAW:
		return setRawCursor();
	default:
		break;
	}
	return false;
}

GLFWcursor* OTTMouse::setCustomCursor(unsigned char* pixels, const int& w, const int& h, const int& x0, const int& y0){
	GLFWimage image;
	image.width = w;
	image.height = h;
	image.pixels = pixels;

	cursor = glfwCreateCursor(&image, x0, y0);
	glfwSetCursor(parent, cursor);
	
	return cursor;
}

void OTTMouse::removeCustomCursor(){
	glfwSetCursor(parent, NULL); // disable custom cursor
	glfwDestroyCursor(cursor);
}

bool OTTMouse::delta() {
	return (dDeltaX != 0 || dDeltaY != 0);
}

void OTTMouse::setPosition(const double& x, const double& y) {
	dDeltaX = x - dPosX;
	dDeltaY = y - dPosY;
	dPosX = x;
	dPosY = y;
}

bool OTTMouse::delta(double& deltaX, double& deltaY) {
	deltaX = dDeltaX;
	deltaY = dDeltaY;
	dDeltaX = 0;
	dDeltaY = 0;
	return (deltaX != 0 || deltaY != 0);
}

bool OTTMouse::poll(const int& button) {
	if (bPressed[button]) {
		bPressed[button] = false;
		return true;
	}
	return false;
}

bool OTTMouse::pressed(const int& button) {
	return poll(button);
}

bool OTTMouse::released(const int& button) {
	if (bReleased[button]) {
		bReleased[button] = false;
		return true;
	}
	return false;
}

void OTTMouse::down(const int& button) {
	if (!bStates[button]) {
		bStates[button] = true;
		bPressed[button] = true;
		bReleased[button] = false;
		nCount++;
	}
}

void OTTMouse::up(const int& button) {
	if (bStates[button]) {
		bStates[button] = false;
		bPressed[button] = false;
		bReleased[button] = true;
		nCount--;
	}
}

void OTTMouse::reset() {
	for (int i = 0; i < 3; i++) {
		bStates[i] = false;
		bPressed[i] = false;
		bReleased[i] = false;
	}
	nCount = 0;
}

void OTTMouse::handleMouse(GLFWwindow* window, int button, int action, int mods){
    //if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
     //   popup_menu(); GLFW_MOUSE_BUTTON_LEFT
	OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
	if(action == GLFW_RELEASE)
		currentWindow->getMouse()->up(button);
	else if(action == GLFW_PRESS)
		currentWindow->getMouse()->down(button);
}

void OTTMouse::handleMouseMove(GLFWwindow* window, double x, double y){
	// Handle active mouse movement (i.e. movement while holding a mouse button)
	OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
	currentWindow->getMouse()->setPosition(x, y);
}

void OTTMouse::handleMouseEnter(GLFWwindow* window, int entered){
	if (entered){ // Cursor entered the window
	}
	else{ // Cursor left the window
	}
}

void OTTMouse::handleMouseScroll(GLFWwindow* window, double xoffset, double yoffset){
}

void OTTMouse::setupCallbacks(bool bEnable/*=true*/) {
	if(bEnable){
		glfwSetMouseButtonCallback(parent, OTTMouse::handleMouse); // Set mouse button callback
		glfwSetCursorPosCallback(parent, OTTMouse::handleMouseMove); // Set mouse movement callback
		glfwSetCursorEnterCallback(parent, OTTMouse::handleMouseEnter); // Set cursor enter / exit callback
		glfwSetScrollCallback(parent, OTTMouse::handleMouseScroll); // Set mouse scroll callback
	}
	else{
		glfwSetMouseButtonCallback(parent, 0x0); // Unset mouse button callback
		glfwSetCursorPosCallback(parent, 0x0); // Unset mouse movement callback
		glfwSetCursorEnterCallback(parent, 0x0); // Unset curser enter / exit callback
		glfwSetScrollCallback(parent, 0x0); // Unset mouse scroll callback	
	}
}

