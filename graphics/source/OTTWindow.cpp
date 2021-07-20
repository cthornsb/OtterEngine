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
	userPathDropCallback(0x0),
	userWindowFocusCallback(0x0),
	userWindowPositionCallback(0x0),
	userWindowSizeCallback(0x0),
	userWindowIconifyCallback(0x0)
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

void OTTWindow::setPathDropCallback(void (*callback)(const std::string&)) {
	userPathDropCallback = callback;
}

void OTTWindow::setWindowFocusCallback(GLFWwindowfocusfun func) {
	userWindowFocusCallback = func;
	glfwSetWindowFocusCallback(win.get(), func);
}

void OTTWindow::setWindowPositionCallback(GLFWwindowposfun func) {
	userWindowPositionCallback = func;
	glfwSetWindowPosCallback(win.get(), func);
}

void OTTWindow::setWindowResizeCallback(GLFWwindowsizefun func) {
	userWindowSizeCallback = func;
	glfwSetWindowSizeCallback(win.get(), func);
}

void OTTWindow::setWindowIconifyCallback(GLFWwindowiconifyfun func) {
	userWindowIconifyCallback = func;
	glfwSetWindowIconifyCallback(win.get(), func);
}

void OTTWindow::setDrawColor(ColorRGB *color){
	glColor4f(color->r / 255.f, color->g / 255.f, color->b / 255.f, color->a / 255.f);
}

void OTTWindow::setDrawColor(const ColorRGB &color){
	glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}

void OTTWindow::resetDrawColor() {
	glColor4f(1.f, 1.f, 1.f, 1.f);
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

void OTTWindow::setWindowTitle(const std::string& title) {
	glfwSetWindowTitle(win.get(), title.c_str());
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

void OTTWindow::drawPixels(const int *x, const int *y, const size_t &N){
	for(size_t i = 0; i < N; i++) // Draw N pixels
		drawPixel(x[i], y[i]);
}

void OTTWindow::drawLine(const int &x0, const int &y0, const int &x1, const int &y1){
	glBegin(GL_LINES);
		glVertex2i(x0, y0);
		glVertex2i(x1, y1);
	glEnd();
}

void OTTWindow::drawLines(const int *x, const int *y, const size_t &N){
	if(N < 2) // Nothing to draw
		return;
	for(size_t i = 0; i < N-1; i++)
		drawLine(x[i], y[i], x[i+1], y[i+1]);
}

void OTTWindow::drawTexture(
	const unsigned int& texture,
	const int& x0, const int& y0,
	const int& x1, const int& y1,
	const int& x2, const int& y2,
	const int& x3, const int& y3
) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0.f, 0.f); glVertex2i(x0, y0);
		glTexCoord2f(1.f, 0.f); glVertex2i(x1, y1);
		glTexCoord2f(1.f, 1.f); glVertex2i(x2, y2);
		glTexCoord2f(0.f, 1.f); glVertex2i(x3, y3);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void OTTWindow::drawTexture(const unsigned int& texture, const int& x0, const int& y0, const int& x1, const int& y1) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0.f, 0.f); glVertex2i(x0, y0);
		glTexCoord2f(1.f, 0.f); glVertex2i(x1, y0);
		glTexCoord2f(1.f, 1.f); glVertex2i(x1, y1);
		glTexCoord2f(0.f, 1.f); glVertex2i(x0, y1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void OTTWindow::drawTexture(const unsigned int& texture) {
	drawTexture(texture, 0, 0, nNativeWidth, nNativeHeight);
}

void OTTWindow::drawTexture(const unsigned int& texture, const ott::TextureModifier& modifier) {
	switch (modifier) {
	case ott::TextureModifier::NONE:
		drawTexture(texture, 0, 0, nNativeWidth, nNativeHeight);
		break;
	case ott::TextureModifier::VFLIP: // Mirror vertically (3, 2, 1, 0)
		drawTextureVertices(texture, 3, 2, 1, 0);
		break;
	case ott::TextureModifier::HFLIP: // Mirror horizontally (1, 0, 3, 2)
		drawTextureVertices(texture, 1, 0, 3, 2);
		break;
	case ott::TextureModifier::ROTATE_CW: // Rotate clockwise 90 degrees (3, 0, 1, 2)
		drawTextureVertices(texture, 3, 0, 1, 2);
		break;
	case ott::TextureModifier::ROTATE_CCW: // Rotate counter-clockwise 90 degrees (1, 2, 3, 0)
		drawTextureVertices(texture, 1, 2, 3, 0);
		break;
	case ott::TextureModifier::ROTATE_180: // Rotate 180 degrees (2, 3, 0, 1)
		drawTextureVertices(texture, 2, 3, 0, 1);
		break;
	default:
		break;
	}
}

void OTTWindow::drawTextureVertices(const unsigned int& texture, const int& i0, const int& i1, const int& i2, const int& i3) {
	//  0    1    2    3
	// 0,0  W,0  W,H  0,H
	const int vertices[4][2] = {
		{0,            0},
		{nNativeWidth, 0},
		{nNativeWidth, nNativeHeight},
		{0,            nNativeHeight}
	};
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0.f, 0.f); glVertex2i(vertices[i0][0], vertices[i0][1]);
		glTexCoord2f(1.f, 0.f); glVertex2i(vertices[i1][0], vertices[i1][1]);
		glTexCoord2f(1.f, 1.f); glVertex2i(vertices[i2][0], vertices[i2][1]);
		glTexCoord2f(0.f, 1.f); glVertex2i(vertices[i3][0], vertices[i3][1]);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void OTTWindow::drawBuffer(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, const OTTImageBuffer* data){
	// Update texture pixels
	//glTextureSubImage2D(nTexture, 0, x0, y0, W, H, GL_RGB, GL_UNSIGNED_BYTE, data->get()); // Since 4.5
	glBindTexture(GL_TEXTURE_2D, nTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x0, y0, W, H, GL_RGB, GL_UNSIGNED_BYTE, data->get()); // Since 2.0
	drawTexture(nTexture, 0, 0, nNativeWidth, nNativeHeight);
}

void OTTWindow::drawBuffer(){
	drawBuffer(nNativeWidth, nNativeHeight, 0, 0, &buffer);	
}

void OTTWindow::buffWrite(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, OTTImageBuffer* data) {
	//if (data->getNumChannels() != buffer.getNumChannels()) // Check for matching color depth
	//	return;
	OTTLogicalColor color;
	for (unsigned short i = 0; i < H; i++) { // Over rows
		for (unsigned short j = 0; j < W; j++) { // Over columns
			if (!data->getPixel(j, i, color))
				continue;
			buffer.setPixel(x0 + j, y0 + i, color);
		}
	}
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
	drawBuffer(nNativeWidth, nNativeHeight, 0, 0, &buffer);
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
	updateWindowSize(width, height, true);

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

	// Further intitialization for derived classes
	onUserInitialize();
	
	if(bFirstInit)
		bFirstInit = false;
		
	return true;
}

void OTTWindow::updatePixelZoom(){
	glPixelZoom((float)width / nNativeWidth, (float)height / nNativeHeight);
}

OTTImageBuffer* OTTWindow::enableImageBuffer(bool bLinearFiltering/* = true*/) {
	if (buffer.isGood()) // Image buffer already enabled
		return &buffer;

	// Setup RAM image buffer
	buffer.resize(nNativeWidth, nNativeHeight, 3); // RGB

	// The image buffer requires the intermediate OpenGL texture buffer
	enableTextureBuffer(bLinearFiltering);

	return &buffer;
}

GLuint OTTWindow::enableTextureBuffer(bool bLinearFiltering/* = true*/) {
	if (nTexture) // Texture buffer already enabled
		return nTexture;

	// Generate new buffer texture
	glGenTextures(1, &nTexture);
	glBindTexture(GL_TEXTURE_2D, nTexture);	
	if(bLinearFiltering) // Results in a softer image, default
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	else // Results in a sharper image when magnified
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, nNativeWidth, nNativeHeight); // Since 4.2

	return nTexture;
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

void OTTWindow::enableAlphaBlending() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OTTWindow::disableAlphaBlending() {
	glDisable(GL_BLEND);
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
	OTTWindow* currentWindow = OTTActiveWindows::get().find(window);
	if (!currentWindow)
		return;
	currentWindow->setWindowFocus(focused == GLFW_TRUE);
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

void OTTWindow::setWindowFocus(const bool& focused) {
	if (userWindowFocusCallback) {
		(*userWindowFocusCallback)(win.get(), focused ? GLFW_TRUE : GLFW_FALSE);
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
	
namespace ott {
	const int MirrorVertical = 0x1; ///< Mirror image or texture about the x-axis
	const int MirrorHorizontal = 0x2; ///< Mirror image or texture about the y-axis
	const int Rotate90DegreesCW = 0x4; ///< Rotate image or texture 90 degrees clockwise
	const int Rotate90DegreesCCW = 0x8; ///< Rotate image or texture 90 degrees counter-clockwise
	const int Rotate180Degrees = 0x10; ///< Rotate image or texture 180 degrees
}
