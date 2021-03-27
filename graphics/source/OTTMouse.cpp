#include "OTTMouse.hpp"
#include "OTTWindow.hpp"

OTTMouse::OTTMouse() :
	nCount(0),
	dPosX(0),
	dPosY(0),
	dDeltaX(0),
	dDeltaY(0),
	bLockPointer(false),
	bStates{ false, false, false },
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
}

void OTTMouse::setDisableCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void OTTMouse::setHideCursor(){
	glfwSetInputMode(parent, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

#ifdef GLFW_RAW_MOUSE_MOTION
bool OTTMouse::setRawCursor(){
	if (glfwRawMouseMotionSupported()){
		setDisableCursor(); // Disable normal cursor
		glfwSetInputMode(parent, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		return true;
	}
	return false;
}
#else
bool OTTMouse::setRawCursor(){
	return false;
}
#endif // ifdef GLFW_RAW_MOUSE_MOTION

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
	/*static bool firstPosition = true;
	if (firstPosition) {
		firstPosition = false;
		OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
		glutWarpPointer(win->getWidth() / 2, win->getHeight() / 2);
	}
	if (!bLockPointer) {
		dDeltaX = x - dPosX;
		dDeltaY = y - dPosY;
		dPosX = x;
		dPosY = y;
	}
	else{
		OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
		dDeltaX = x - win->getWidth() / 2;
		dDeltaY = y - win->getHeight() / 2;
		glutWarpPointer(win->getWidth() / 2, win->getHeight() / 2);
	}*/
}

bool OTTMouse::delta(double& deltaX, double& deltaY) {
	deltaX = dDeltaX;
	deltaY = dDeltaY;
	dDeltaX = 0;
	dDeltaY = 0;
	return (deltaX != 0 || deltaY != 0);
}

bool OTTMouse::poll(const int& button) {
	if (bStates[button]) {
		bStates[button] = false;
		return true;
	}
	return false;
}

void OTTMouse::down(const int& button) {
	if (!bStates[button]) {
		bStates[button] = true;
		nCount++;
	}
}

void OTTMouse::up(const int& button) {
	if (bStates[button]) {
		bStates[button] = false;
		nCount--;
	}
}

void OTTMouse::reset() {
	for (int i = 0; i < 3; i++)
		bStates[i] = false;
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
		glfwSetCursorEnterCallback(parent, OTTMouse::handleMouseEnter); // Set curser enter / exit callback
		glfwSetScrollCallback(parent, OTTMouse::handleMouseScroll); // Set mouse scroll callback
	}
	else{
		glfwSetMouseButtonCallback(parent, 0x0); // Unset mouse button callback
		glfwSetCursorPosCallback(parent, 0x0); // Unset mouse movement callback
		glfwSetCursorEnterCallback(parent, 0x0); // Unset curser enter / exit callback
		glfwSetScrollCallback(parent, 0x0); // Unset mouse scroll callback	
	}
}

