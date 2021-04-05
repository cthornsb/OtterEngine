#include <iostream>

#include "OTTWindow.hpp"
#include "OTTJoypad.hpp"
#include "OTTTexture.hpp"

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cout << " [opengl] debug: " << message << std::endl;
}

OTTWindow::OTTWindow(const int &w, const int &h, const int& scale/*=1*/) : 
	win(),
	monitor(0x0),
	nTexture(0),
	nNativeWidth(w), 
	nNativeHeight(h),
	fNativeAspect(float(w)/h),
	width(nNativeWidth * scale),
	height(nNativeHeight * scale),
	aspect(fNativeAspect),
	nOffsetX(0),
	nOffsetY(0),
	nOldWidth(width),
	nOldHeight(height),
	nOldPosX(0),
	nOldPosY(0),
	init(false),
	bFirstInit(true),
	bLockAspectRatio(false),
	bFullScreenMode(false),
	bVSync(false),
	bDebugMode(false),
	mOrthoProjection(identityMatrix4),
	keys(),
	mouse(),
	joypad(&OTTJoypad::getInstance()),
	buffer(),
	previousMouseState(MouseStates::NORMAL),
	userPathDropCallback(0x0)
{
}

OTTWindow::~OTTWindow(){
	// glfw window will automatically be destroyed by its unique_ptr destructor
	if (nTexture) { // Delete frame buffer texture
		glDeleteTextures(1, &nTexture);
	}
}

void OTTWindow::close(){
	glfwSetWindowShouldClose(win.get(), GLFW_TRUE);
}

bool OTTWindow::processEvents(){
	if(!status())
		return false;
	if(joypad->isConnected())
		joypad->update();
	glfwPollEvents();
	return true;
}

std::string OTTWindow::getClipboardString() const {
	const char* str = glfwGetClipboardString(NULL);
	if(str)
		return std::string(str);
	return "";
}

int OTTWindow::getAvailableVideoModes(std::vector<OTTWindow::VideoMode>& modes) const {
 	if(!monitor)
 		return -1;
	int count;
	const GLFWvidmode* arr = glfwGetVideoModes(monitor, &count);
	for(int i = 0; i < count; i++)
		modes.push_back(&arr[i]);
	return count;
}

bool OTTWindow::getCurrentVideoMode(OTTWindow::VideoMode& mode) const {
 	if(!monitor)
 		return false;
	mode = OTTWindow::VideoMode(glfwGetVideoMode(monitor));
	return true;
}

void OTTWindow::printVideoModes() const {
	OTTWindow::VideoMode currentMode;
	if(!getCurrentVideoMode(currentMode))
		return;
	std::cout << " Current Mode: " << currentMode.nWidth << "x" << currentMode.nHeight << ", " << currentMode.nColorDepth << "-bit @ " << currentMode.nRefreshRate << " Hz" << std::endl;
	std::cout << " Available Modes:" << std::endl;
	std::cout << "  W\tH\tDepth\tRefresh" << std::endl;
	std::vector<OTTWindow::VideoMode> modes;
	getAvailableVideoModes(modes);
	for(auto mode = modes.cbegin(); mode != modes.cend(); mode++){
		std::cout << "  " << mode->nWidth << "\t" << mode->nHeight << "\t" << mode->nColorDepth << "-bit\t" << mode->nRefreshRate << " Hz" << std::endl;
	}
}

void OTTWindow::updateWindowSize(const int& w, const int& h, bool updatePhysicalSize/*=false*/){
	if (!init)
		return;
	width = w;
	height = (h > 0 ? h : 1); // Prevent division by zero
	aspect = float(w) / float(h);
	nOffsetX = 0;
	nOffsetY = 0;
	if (!bLockAspectRatio)
		bLockAspectRatio = true;
	if(bLockAspectRatio){ // Preserve original aspect ratio		
		if(aspect >= fNativeAspect){ // New window is too wide (pillarbox)
			float wprime = fNativeAspect * height;
			nOffsetX = (int)((width - wprime) / 2.f);
			nOffsetY = 0;
			width = (int)wprime;
		}
		else{ // New window is too tall (letterbox)
			float hprime = width / fNativeAspect;
			nOffsetX = 0;
			nOffsetY = (int)((height - hprime) / 2.f);
			height = (int)hprime;
		}
		aspect = float(width) / float(height);
	}
	if(updatePhysicalSize)
		glfwSetWindowSize(win.get(), width, height); // Update physical window size
	onUserReshape();
}

void OTTWindow::updateWindowSize(const int& scale){
	updateWindowSize(nNativeWidth * scale, nNativeHeight * scale, true);
}

void OTTWindow::setClipboardString(const std::string& str) const {
	glfwSetClipboardString(NULL, str.c_str());
}

void OTTWindow::setPathDropCallback(void (*callback)(const std::string&)){
	userPathDropCallback = callback;
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

void OTTWindow::setFullScreenMode(bool state/*=true*/){
	bFullScreenMode = state;
	if(init){
		if(bFullScreenMode){ // Windowed full screen
			monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			nOldWidth = width;
			nOldHeight = height;
			glfwGetWindowPos(win.get(), &nOldPosX, &nOldPosY);
			glfwSetWindowMonitor(win.get(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			previousMouseState = mouse.getCursorState();
			mouse.setHideCursor();
		}
		else{ // Windowed
			updateWindowSize(nOldWidth, nOldHeight);
			glfwSetWindowMonitor(win.get(), NULL, nOldPosX, nOldPosY, width, height, GLFW_DONT_CARE);
			mouse.setCursorState(previousMouseState);
		}
		if (bVSync)
			enableVSync();
		else
			disableVSync();
	}
}

bool OTTWindow::toggleFullScreenMode(){
	if(bFullScreenMode)
		setFullScreenMode(false);
	else
		setFullScreenMode(true);
	return bFullScreenMode;
}

void OTTWindow::changeVideoMode(const OTTWindow::VideoMode& mode){
	if(!init || !bFullScreenMode)
		return;
	monitor = glfwGetPrimaryMonitor();
	glfwSetWindowMonitor(win.get(), monitor, 0, 0, mode.nWidth, mode.nHeight, mode.nRefreshRate);
}

void OTTWindow::clear(const ColorRGB &color/*=Colors::BLACK*/){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void OTTWindow::drawTexture(const unsigned int& texture, const int& x1, const int& y1, const int& x2, const int& y2) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex2i(x1, y1);
		glTexCoord2i(1, 1); glVertex2i(x2, y1);
		glTexCoord2i(1, 0); glVertex2i(x2, y2);
		glTexCoord2i(0, 0); glVertex2i(x1, y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OTTWindow::drawTexture(const unsigned int& texture) {
	drawTexture(texture, 0, 0, nNativeWidth, nNativeHeight);
}

void OTTWindow::drawBitmap(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, const unsigned char* data){
	glRasterPos2i(x0, y0);
	glBitmap(W, H, 0, 0, 0, 0, data);
}

void OTTWindow::drawBuffer(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, const OTTImageBuffer* data){
	// Update texture pixels
#if (OPENGL_VERSION_MAJOR >= 4 && OPENGL_VERSION_MINOR >= 5)
	glTextureSubImage2D(nTexture, 0, x0, y0, W, H, GL_RGB, GL_UNSIGNED_BYTE, data->get()); // Since 4.5
#else
	glBindTexture(GL_TEXTURE_2D, nTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x0, y0, W, H, GL_RGB, GL_UNSIGNED_BYTE, data->get()); // Since 2.0
	glBindTexture(GL_TEXTURE_2D, 0);
#endif // if (OPENGL_VERSION >= 4.5)
}

void OTTWindow::drawBuffer(){
	drawBuffer(nNativeWidth, nNativeHeight, 0, 0, &buffer);
	drawTexture(nTexture, 0, 0, nNativeWidth, nNativeHeight);
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

void OTTWindow::renderBuffer(){
	drawBuffer();
	render();
}

bool OTTWindow::status(){
	return (init && !glfwWindowShouldClose(win.get()));
}

bool OTTWindow::initialize(const std::string& name){
	if(init || nNativeWidth == 0 || nNativeHeight == 0) 
		return false;

	// Set the GLFW error callback
	if(bDebugMode)
		glfwSetErrorCallback(handleErrors);

	// Open the graphics window
	if (bFirstInit)
		glfwInit();

	// Enable OpenGL debug output
	if (bDebugMode) 
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// Create the window
	if(!bFullScreenMode) // Windowed mode
		win.reset(glfwCreateWindow(nNativeWidth, nNativeHeight, name.c_str(), NULL, NULL)); // Windowed
	else // Full screen mode
		win.reset(glfwCreateWindow(nNativeWidth, nNativeHeight, name.c_str(), glfwGetPrimaryMonitor(), NULL)); // Fullscreen
	
	if(!win.get()) // Window creation failed
		return false;

	// Set the primary monitor pointer
	monitor = glfwGetPrimaryMonitor();
	
	// Add window to the map
	OTTActiveWindows::get().add(win.get(), this);

	// Set initialization flag
	init = true;

	// Update projection matrix
	updateWindowSize(width, height);

	// Set window size handler
	glfwSetWindowSizeCallback(win.get(), OTTWindow::handleReshapeScene);

	// Set window focus callback
	glfwSetWindowFocusCallback(win.get(), OTTWindow::handleWindowFocus);
	
	// Set path / directory drop callback
	glfwSetDropCallback(win.get(), OTTWindow::handlePathDrop);
	
	// Link to mouse and keyboard callback handlers
	mouse.setParentWindow(win.get());
	keys.setParentWindow(win.get());
	
	// Initialize GLEW
	if (bFirstInit) {
		setCurrent();
		GLint err = glewInit();
		if (err != GLEW_OK) {
			const GLubyte* str = glewGetErrorString(err);
			std::cout << " [glew] Error! id=" << err << " : " << str << std::endl;
		}
	}

	if (bDebugMode) {
		// OpenGL debug message callback (since 4.3)
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
	}

	// Setup frame buffer
	buffer.resize(nNativeWidth, nNativeHeight);

	// Generate new buffer texture
	glGenTextures(1, &nTexture);
	glBindTexture(GL_TEXTURE_2D, nTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Results in a sharper image when magnified
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Results in a softer image, default
#if (OPENGL_VERSION_MAJOR >= 4 && OPENGL_VERSION_MINOR >= 2)
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, nNativeWidth, nNativeHeight); // Since 4.2
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, nNativeWidth, nNativeHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // Since 2.0
#endif // if (OPENGL_VERSION >= 4.2)
	glBindTexture(GL_TEXTURE_2D, 0);

	// Further intitialization for derived classes
	onUserInitialize();
	
	if(bFirstInit)
		bFirstInit = false;
		
	return true;
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

void OTTWindow::enableGamepad() {
	joypad->enable();
}

void OTTWindow::disableGamepad() {
	joypad->disable();
}

void OTTWindow::enableVSync() {
/*#ifndef WIN32
	glXSwapIntervalSGI(1);
	//glXSwapIntervalMESA(1);
#else
	wglSwapIntervalEXT(1);
#endif // ifndef WIN32*/
	setCurrent();
	glfwSwapInterval(1);
	bVSync = true;
}

void OTTWindow::disableVSync() {
/*#ifndef WIN32
	glXSwapIntervalSGI(0);
	//glXSwapIntervalMESA(0);
#else
	wglSwapIntervalEXT(0);
#endif // ifndef WIN32*/
	setCurrent();
	glfwSwapInterval(0);
	bVSync = false;
}

bool OTTWindow::saveImageBufferToBitmap(const std::string& fname){
	return OTTTexture::write(buffer, fname, false, true);
}

bool OTTWindow::saveFrameBufferToBitmap(const std::string& fname){
	unsigned char* data = 0x0;
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	delete[] data;
	return false;
}

void OTTWindow::handleErrors(int error, const char* description) {
	std::cout << " [glfw] Error! id=" << error << " : " << description << std::endl;
}

void OTTWindow::handleReshapeScene(GLFWwindow* window, int W, int H){
	OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
	if(!currentWindow)
		return;
	
	// Update window size
	currentWindow->updateWindowSize(W, H);
}

void OTTWindow::handleWindowFocus(GLFWwindow* window, int focused){
	if (focused){ // The window gained input focus
	}
	else{ // The window lost input focus
	}
}

void OTTWindow::handlePathDrop(GLFWwindow* window, int count, const char** paths){
	OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
	if(!currentWindow)
		return;
	std::vector<std::string> pathStrs;
	for(int i = 0; i < count; i++){
		pathStrs.push_back(std::string(paths[i]));
	}
	currentWindow->dropSystemPaths(pathStrs);
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
	glViewport(nOffsetX, nOffsetY, width, height); // x, y, width, height
	glOrtho(0, nNativeWidth, nNativeHeight, 0, -1, 1); // left, right, bottom, top, near, far (deprecated)
	glMatrixMode(GL_MODELVIEW);

	// Orthographic projection matrix
	mOrthoProjection = Matrix4::getOrthographicMatrix(0.f, (float)nNativeWidth, (float)nNativeHeight, 0.f, -1.f, 1.f); // left, right, bottom, top, near, far

	// Clear the window
	clear();
}

void OTTWindow::dropSystemPaths(const std::vector<std::string>& paths){
	if(userPathDropCallback){
		for(auto str = paths.cbegin(); str != paths.cend(); str++)
			(*userPathDropCallback)(*str);
	}
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
	
