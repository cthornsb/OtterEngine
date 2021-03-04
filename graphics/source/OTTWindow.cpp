#include <iostream>

#include "OTTWindow.hpp"

OTTWindow::~OTTWindow(){
	// glfw window will automatically be destroyed by its unique_ptr destructor
}

void OTTWindow::close(){
	glfwSetWindowShouldClose(win.get(), GLFW_TRUE);
}

bool OTTWindow::processEvents(){
	if(!status())
		return false;
	glfwPollEvents();
	return true;
}

void OTTWindow::updateWindowSize(const int& w, const int& h){
	width = w;
	height = (h > 0 ? h : 1); // Prevent division by zero
	aspect = float(w) / float(h);
	if(init){
		glfwSetWindowSize(win.get(), width, height); // Update physical window size
		onUserReshape();
	}
}

void OTTWindow::updateWindowSize(const int& scale){
	updateWindowSize(nNativeWidth * scale, nNativeHeight * scale);
}

void OTTWindow::setDrawColor(ColorRGB *color, const float &alpha/*=1*/){
	glColor4f(color->r, color->g, color->b, color->a);
}

void OTTWindow::setDrawColor(const ColorRGB &color, const float &alpha/*=1*/){
	glColor4f(color.r, color.g, color.b, color.a);
}

void OTTWindow::setCurrent(){
	glfwMakeContextCurrent(win.get());
}

void OTTWindow::clear(const ColorRGB &color/*=Colors::BLACK*/){
	glClear(GL_COLOR_BUFFER_BIT);
}

void OTTWindow::drawPixel(const int &x, const int &y){
	glBegin(GL_POINTS);
		glVertex2i(x, y);
	glEnd();
}

void OTTWindow::drawPixel(const int *x, const int *y, const size_t &N){
	for(size_t i = 0; i < N; i++) // Draw N pixels
		drawPixel(x[i], y[i]);
}

void OTTWindow::drawLine(const int &x1, const int &y1, const int &x2, const int &y2){
	glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
	glEnd();
}

void OTTWindow::drawLine(const int *x, const int *y, const size_t &N){
	if(N < 2) // Nothing to draw
		return;
	for(size_t i = 0; i < N-1; i++)
		drawLine(x[i], y[i], x[i+1], y[i+1]);
}

void OTTWindow::drawRectangle(const int &x1, const int &y1, const int &x2, const int &y2){
	drawLine(x1, y1, x2, y1); // Top
	drawLine(x2, y1, x2, y2); // Right
	drawLine(x2, y2, x1, y2); // Bottom
	drawLine(x1, y2, x1, y1); // Left
}

void OTTWindow::drawBitmap(const unsigned int& width, const unsigned int& height, const float& x0, const float& y0, const unsigned char* data){
	glRasterPos2i(x0, y0);
	glBitmap(width, height, 0, 0, 0, 0, data);
}

void OTTWindow::drawPixels(const unsigned int& width, const unsigned int& height, const float& x0, const float& y0, const OTTImageBuffer* data){
	glRasterPos2i(x0, y0);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, data->get());
}

void OTTWindow::buffWrite(const unsigned short& x, const unsigned short& y, const ColorRGB& color){
	buffer.setPixel(x, y, color);
}

void OTTWindow::buffWriteLine(const unsigned short& y, const ColorRGB& color){
	buffer.setPixelRow(y, color);
}

void OTTWindow::render(){
	glFlush();
	glfwSwapBuffers(win.get());
}

void OTTWindow::drawBuffer(){
	drawPixels(nNativeWidth, nNativeHeight ,0, nNativeHeight, &buffer);
}

void OTTWindow::renderBuffer(){
	drawBuffer();
	render();
}

bool OTTWindow::status(){
	return (init && !glfwWindowShouldClose(win.get()));
}

void OTTWindow::initialize(const std::string& name){
	if(init || nNativeWidth == 0 || nNativeHeight == 0) 
		return;

	// Set the GLFW error callback
	glfwSetErrorCallback(handleErrors);

	// Open the graphics window
	if(bFirstInit)
		glfwInit();
	
	// Create the window
	win.reset(glfwCreateWindow(nNativeWidth, nNativeHeight, name.c_str(), NULL, NULL)); // Windowed
	//win.reset(glfwCreateWindow(nNativeWidth, nNativeHeight, name.c_str(), glfwGetPrimaryMonitor(), NULL)); // Fullscreen
	
	// Add window to the map
	OTTActiveWindows::get().add(win.get(), this);

	// Setup frame buffer
	buffer.resize(nNativeWidth, nNativeHeight);

	// Set initialization flag
	init = true;

	// Update projection matrix
	updateWindowSize(width, height);

	// Set window size handler
	glfwSetWindowSizeCallback(win.get(), OTTWindow::handleReshapeScene);

	// Set window focus callback
	glfwSetWindowFocusCallback(win.get(), OTTWindow::handleWindowFocus);
	
	// Link to mouse and keyboard callback handlers
	mouse.setParentWindow(win.get());
	keys.setParentWindow(win.get());
	
	// Further intitialization for derived classes
	onUserInitialize();
	
	if(bFirstInit)
		bFirstInit = false;
}

void OTTWindow::updatePixelZoom(){
	glPixelZoom((float)width / nNativeWidth, (float)height / nNativeHeight);
}

void OTTWindow::enableKeyboard() {
	keys.enable();
}

void OTTWindow::disableKeyboard() {
	keys.disable();
}

void OTTWindow::enableMouse() {
	mouse.enable();
}

void OTTWindow::disableMouse() {
	mouse.disable();
}

void OTTWindow::handleErrors(int error, const char* description) {
	std::cout << " [glfw] Error! id=" << error << " : " << description << std::endl;
}

void OTTWindow::handleReshapeScene(GLFWwindow* window, int width, int height){
	OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
	if(!currentWindow)
		return;
	
	// Update window size
	currentWindow->updateWindowSize(width, height);
}

void OTTWindow::handleWindowFocus(GLFWwindow* window, int focused){
	if (focused){ // The window gained input focus
	}
	else{ // The window lost input focus
	}
}

void OTTWindow::setKeyboardStreamMode(){
	// Enable keyboard repeat
	keys.enableStreamMode();
}

void OTTWindow::setKeyboardToggleMode(){
	// Disable keyboard repeat
	keys.disableStreamMode();
}

void OTTWindow::reshape(){
	setCurrent();
	
	updatePixelZoom();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Update viewport
	glViewport(0, 0, width, height);
	glOrtho(0, nNativeWidth, nNativeHeight, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	// Clear the window.
	clear();
}

OTTActiveWindows& OTTActiveWindows::get(){
	static OTTActiveWindows instance;
	return instance;
}

void OTTActiveWindows::add(GLFWwindow* glfw, OTTWindow* win){
	listOfWindows[glfw] = win;
}

OTTWindow* OTTActiveWindows::find(GLFWwindow* glfw){
	OTTWindow* retval = 0x0;
	auto win = listOfWindows.find(glfw);
	if(win != listOfWindows.end())
		retval = win->second;
	return retval;
}
	
