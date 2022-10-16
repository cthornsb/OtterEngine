#include "OTTWindow.hpp"
#include "OTTColor.hpp"
#include "OTTLogicalColor.hpp"

#include <input/OTTJoypad.hpp>
#include <input/OTTKeyboard.hpp>
#include <input/OTTMouse.hpp>
#include <graphics/texture/OTTTexture.hpp>

#include <iostream>

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

ott::Window::Window(const int &w, const int &h, const int& scale/*=1*/) : 
	win(),
	monitor(nullptr),
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
	mOrthoProjection(constants::kIdentityMatrix4),
	keys(&ott::Keyboard::Instance()),
	mouse(&ott::Mouse::Instance()),
	joypad(&ott::Joypad::Instance()),
	buffer(),
	previousMouseState(MouseStates::Normal),
	userPathDropCallback(nullptr),
	userWindowFocusCallback(nullptr),
	userWindowPositionCallback(nullptr),
	userWindowSizeCallback(nullptr),
	userWindowIconifyCallback(nullptr)
{
}

ott::Window::~Window(){
	// glfw window will automatically be destroyed by its unique_ptr destructor
	if (nTexture) { // Delete frame buffer texture
		glDeleteTextures(1, &nTexture);
	}
}

void ott::Window::Close(){
	glfwSetWindowShouldClose(win.get(), GLFW_TRUE);
}

bool ott::Window::ProcessEvents(){
	if (this->Status() == false) {
		return false;
	}
	if (joypad->Connected() == true) {
		joypad->Update();
	}
	glfwPollEvents();
	return true;
}

std::string ott::Window::ClipboardString() const {
	const char* str = glfwGetClipboardString(NULL);
	if (str) {
		return std::string(str);
	}
	return "";
}

int ott::Window::AvailableVideoModes(std::vector<ott::Window::VideoMode_t>& modes) const {
	if (monitor == false) {
		return -1;
	}
	int count;
	const GLFWvidmode* arr = glfwGetVideoModes(monitor, &count);
	for (int i = 0; i < count; i++) {
		modes.push_back(&arr[i]);
	}
	return count;
}

bool ott::Window::CurrentVideoMode(ott::Window::VideoMode_t& mode) const {
	if (monitor == false) {
		return false;
	}
	mode = ott::Window::VideoMode_t(glfwGetVideoMode(monitor));
	return true;
}

void ott::Window::PrintVideoModes() const {
	ott::Window::VideoMode_t currentMode;
	if (this->CurrentVideoMode(currentMode) == false) {
		return;
	}
	std::cout << " Current Mode: " << currentMode.nWidth << "x" << currentMode.nHeight << ", " << currentMode.nColorDepth << "-bit @ " << currentMode.nRefreshRate << " Hz" << std::endl;
	std::cout << " Available Modes:" << std::endl;
	std::cout << "  W\tH\tDepth\tRefresh" << std::endl;
	std::vector<ott::Window::VideoMode_t> modes;
	this->AvailableVideoModes(modes);
	for(auto mode = modes.cbegin(); mode != modes.cend(); mode++){
		std::cout << "  " << mode->nWidth << "\t" << mode->nHeight << "\t" << mode->nColorDepth << "-bit\t" << mode->nRefreshRate << " Hz" << std::endl;
	}
}

void ott::Window::UpdateWindowSize(const int& w, const int& h, bool updatePhysicalSize/*=false*/){
	if (init == false) {
		return;
	}
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
	if (updatePhysicalSize == true) {
		glfwSetWindowSize(win.get(), width, height); // Update physical window size
	}
	this->OnUserReshape();
}

void ott::Window::UpdateWindowSize(const int& scale){
	this->UpdateWindowSize(nNativeWidth * scale, nNativeHeight * scale, true);
}

void ott::Window::SetClipboardString(const std::string& str) const {
	glfwSetClipboardString(NULL, str.c_str());
}

void ott::Window::SetPathDropCallback(void (*callback)(const std::string&)) {
	userPathDropCallback = callback;
}

void ott::Window::SetWindowFocusCallback(GLFWwindowfocusfun func) {
	userWindowFocusCallback = func;
	glfwSetWindowFocusCallback(win.get(), func);
}

void ott::Window::SetWindowPositionCallback(GLFWwindowposfun func) {
	userWindowPositionCallback = func;
	glfwSetWindowPosCallback(win.get(), func);
}

void ott::Window::SetWindowResizeCallback(GLFWwindowsizefun func) {
	userWindowSizeCallback = func;
	glfwSetWindowSizeCallback(win.get(), func);
}

void ott::Window::SetWindowIconifyCallback(GLFWwindowiconifyfun func) {
	userWindowIconifyCallback = func;
	glfwSetWindowIconifyCallback(win.get(), func);
}

void ott::Window::SetDrawColor(ColorRGB *color){
	glColor4f(color->r / 255.f, color->g / 255.f, color->b / 255.f, color->a / 255.f);
}

void ott::Window::SetDrawColor(const ColorRGB &color){
	glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}

void ott::Window::ResetDrawColor() {
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

void ott::Window::SetCurrent(){
	glfwMakeContextCurrent(win.get());
}

void ott::Window::SetFullScreenMode(bool state/*=true*/){
	bFullScreenMode = state;
	if(init){
		if(bFullScreenMode){ // Windowed full screen
			monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			nOldWidth = width;
			nOldHeight = height;
			glfwGetWindowPos(win.get(), &nOldPosX, &nOldPosY);
			glfwSetWindowMonitor(win.get(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			previousMouseState = mouse->CursorState();
			mouse->HideCursor();
		}
		else{ // Windowed
			this->UpdateWindowSize(nOldWidth, nOldHeight);
			glfwSetWindowMonitor(win.get(), NULL, nOldPosX, nOldPosY, width, height, GLFW_DONT_CARE);
			mouse->SetCursorState(previousMouseState);
		}
		if (bVSync) {
			this->EnableVSync();
		}
		else {
			this->DisableVSync();
		}
	}
}

void ott::Window::SetWindowTitle(const std::string& title) {
	glfwSetWindowTitle(win.get(), title.c_str());
}

bool ott::Window::ToggleFullScreenMode(){
	if (bFullScreenMode) {
		this->SetFullScreenMode(false);
	}
	else {
		this->SetFullScreenMode(true);
	}
	return bFullScreenMode;
}

void ott::Window::ChangeVideoMode(const ott::Window::VideoMode_t& mode){
	if(!init || !bFullScreenMode)
		return;
	monitor = glfwGetPrimaryMonitor();
	glfwSetWindowMonitor(win.get(), monitor, 0, 0, mode.nWidth, mode.nHeight, mode.nRefreshRate);
}

void ott::Window::Clear() {
	this->Clear(colors::Black);
}

void ott::Window::Clear(const ColorRGB &color/*=Colors::BLACK*/){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ott::Window::DrawPixel(const int &x, const int &y){
	glBegin(GL_POINTS);
		glVertex2i(x, y);
	glEnd();
}

void ott::Window::DrawPixels(const int *x, const int *y, const size_t &N){
	for (size_t i = 0; i < N; i++) { // Draw N pixels
		this->DrawPixel(x[i], y[i]);
	}
}

void ott::Window::DrawLine(const int &x0, const int &y0, const int &x1, const int &y1){
	glBegin(GL_LINES);
		glVertex2i(x0, y0);
		glVertex2i(x1, y1);
	glEnd();
}

void ott::Window::DrawLines(const int *x, const int *y, const size_t &N){
	if (N < 2) { // Nothing to draw
		return;
	}
	for (size_t i = 0; i < N - 1; i++) {
		this->DrawLine(x[i], y[i], x[i + 1], y[i + 1]);
	}
}

void ott::Window::DrawTexture(
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

void ott::Window::DrawTexture(const unsigned int& texture, const int& x0, const int& y0, const int& x1, const int& y1) {
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

void ott::Window::DrawTexture(const unsigned int& texture) {
	this->DrawTexture(texture, 0, 0, nNativeWidth, nNativeHeight);
}

void ott::Window::DrawTexture(const unsigned int& texture, const ott::TextureModifier& modifier) {
	switch (modifier) {
	case ott::TextureModifier::None:
		this->DrawTexture(texture, 0, 0, nNativeWidth, nNativeHeight);
		break;
	case ott::TextureModifier::VFlip: // Mirror vertically (3, 2, 1, 0)
		this->DrawTextureVertices(texture, 3, 2, 1, 0);
		break;
	case ott::TextureModifier::HFlip: // Mirror horizontally (1, 0, 3, 2)
		this->DrawTextureVertices(texture, 1, 0, 3, 2);
		break;
	case ott::TextureModifier::RotateCW: // Rotate clockwise 90 degrees (3, 0, 1, 2)
		this->DrawTextureVertices(texture, 3, 0, 1, 2);
		break;
	case ott::TextureModifier::RotateCCW: // Rotate counter-clockwise 90 degrees (1, 2, 3, 0)
		this->DrawTextureVertices(texture, 1, 2, 3, 0);
		break;
	case ott::TextureModifier::Rotate180: // Rotate 180 degrees (2, 3, 0, 1)
		this->DrawTextureVertices(texture, 2, 3, 0, 1);
		break;
	default:
		break;
	}
}

void ott::Window::DrawTextureVertices(const unsigned int& texture, const int& i0, const int& i1, const int& i2, const int& i3) {
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

void ott::Window::DrawBuffer(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, const ImageBuffer* data){
	// Update texture pixels
	//glTextureSubImage2D(nTexture, 0, x0, y0, W, H, GL_RGB, GL_UNSIGNED_BYTE, data->Get()); // Since 4.5
	glBindTexture(GL_TEXTURE_2D, nTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x0, y0, W, H, GL_RGB, GL_UNSIGNED_BYTE, data->Get()); // Since 2.0
	this->DrawTexture(nTexture, 0, 0, nNativeWidth, nNativeHeight);
}

void ott::Window::DrawBuffer(){
	this->DrawBuffer(nNativeWidth, nNativeHeight, 0, 0, &buffer);
}

void ott::Window::BuffWrite(const unsigned int& W, const unsigned int& H, const int& x0, const int& y0, ImageBuffer* data) {
	//if (data->getNumChannels() != buffer.getNumChannels()) // Check for matching color depth
	//	return;
	LogicalColor color;
	for (unsigned short i = 0; i < H; i++) { // Over rows
		for (unsigned short j = 0; j < W; j++) { // Over columns
			if (!data->GetPixel(j, i, color))
				continue;
			buffer.SetPixel(x0 + j, y0 + i, color);
		}
	}
}

void ott::Window::BuffWrite(const unsigned short& x, const unsigned short& y, const ColorRGB& color){
	buffer.SetPixel(x, y, color);
}

void ott::Window::BuffWriteLine(const unsigned short& y, const ColorRGB& color){
	buffer.SetPixelRow(y, color);
}

void ott::Window::Render(){
	glFlush();
	glfwSwapBuffers(win.get());
}

void ott::Window::RenderBuffer(){
	this->DrawBuffer(nNativeWidth, nNativeHeight, 0, 0, &buffer);
	this->Render();
}

bool ott::Window::Status(){
	return (init && !glfwWindowShouldClose(win.get()));
}

bool ott::Window::Initialize(const std::string& name){
	if(init || nNativeWidth == 0 || nNativeHeight == 0) 
		return false;

	// Set the GLFW error callback
	if(bDebugMode)
		glfwSetErrorCallback(HandleErrors);

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
	ActiveWindows::Instance().Add(win.get(), this);

	// Set initialization flag
	init = true;

	// Update projection matrix
	this->UpdateWindowSize(width, height, true);

	// Set window size handler
	glfwSetWindowSizeCallback(win.get(), ott::Window::HandleReshapeScene);

	// Set window focus callback
	glfwSetWindowFocusCallback(win.get(), ott::Window::HandleWindowFocus);
	
	// Set path / directory drop callback
	glfwSetDropCallback(win.get(), ott::Window::HandlePathDrop);
	
	// Link to mouse and keyboard callback handlers
	mouse->SetParentWindow(win.get());
	keys->SetParentWindow(win.get());
	
	// Initialize GLEW
	if (bFirstInit) {
		this->SetCurrent();
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
	this->OnUserInitialize();
	
	if(bFirstInit)
		bFirstInit = false;
		
	return true;
}

void ott::Window::UpdatePixelZoom(){
	glPixelZoom((float)width / nNativeWidth, (float)height / nNativeHeight);
}

ott::ImageBuffer* ott::Window::EnableImageBuffer(bool bLinearFiltering/* = true*/) {
	if (buffer.Good() == true) { // Image buffer already enabled
		return &buffer;
	}

	// Setup RAM image buffer
	buffer.Resize(nNativeWidth, nNativeHeight, 3); // RGB

	// The image buffer requires the intermediate OpenGL texture buffer
	this->EnableTextureBuffer(bLinearFiltering);

	return &buffer;
}

GLuint ott::Window::EnableTextureBuffer(bool bLinearFiltering/* = true*/) {
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

void ott::Window::EnableKeyboard() {
	keys->Enable();
}

void ott::Window::DisableKeyboard() {
	keys->Disable();
}

void ott::Window::EnableMouse() {
	mouse->Enable();
}

void ott::Window::DisableMouse() {
	mouse->Disable();
}

void ott::Window::EnableGamepad() {
	joypad->Enable();
}

void ott::Window::DisableGamepad() {
	joypad->Disable();
}

void ott::Window::EnableVSync() {
/*#ifndef WIN32
	glXSwapIntervalSGI(1);
	//glXSwapIntervalMESA(1);
#else
	wglSwapIntervalEXT(1);
#endif // ifndef WIN32*/
	this->SetCurrent();
	glfwSwapInterval(1);
	bVSync = true;
}

void ott::Window::DisableVSync() {
/*#ifndef WIN32
	glXSwapIntervalSGI(0);
	//glXSwapIntervalMESA(0);
#else
	wglSwapIntervalEXT(0);
#endif // ifndef WIN32*/
	this->SetCurrent();
	glfwSwapInterval(0);
	bVSync = false;
}

void ott::Window::EnableAlphaBlending() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ott::Window::DisableAlphaBlending() {
	glDisable(GL_BLEND);
}

void ott::Window::HandleErrors(int error, const char* description) {
	std::cout << " [glfw] Error! id=" << error << " : " << description << std::endl;
}

void ott::Window::HandleReshapeScene(GLFWwindow* window, int W, int H){
	Window* currentWindow = ActiveWindows::Instance().Find(window);
	if(!currentWindow)
		return;
	
	// Update window size
	currentWindow->UpdateWindowSize(W, H);
}

void ott::Window::HandleWindowFocus(GLFWwindow* window, int focused){
	Window* currentWindow = ActiveWindows::Instance().Find(window);
	if (!currentWindow)
		return;
	currentWindow->SetWindowFocus(focused == GLFW_TRUE);
}

void ott::Window::HandlePathDrop(GLFWwindow* window, int count, const char** paths){
	Window* currentWindow = ActiveWindows::Instance().Find(window);
	if(!currentWindow)
		return;
	std::vector<std::string> pathStrs;
	for(int i = 0; i < count; i++){
		pathStrs.push_back(std::string(paths[i]));
	}
	currentWindow->DropSystemPaths(pathStrs);
}

void ott::Window::SetKeyboardStreamMode(){
	// Enable keyboard repeat
	keys->EnableStreamMode();
}

void ott::Window::SetKeyboardToggleMode(){
	// Disable keyboard repeat
	keys->DisableStreamMode();
}

void ott::Window::Reshape(){
	this->SetCurrent();
	
	this->UpdatePixelZoom();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Update viewport
	glViewport(nOffsetX, nOffsetY, width, height); // x, y, width, height
	glOrtho(0, nNativeWidth, nNativeHeight, 0, -1, 1); // left, right, bottom, top, near, far (deprecated)
	glMatrixMode(GL_MODELVIEW);

	// Orthographic projection matrix
	mOrthoProjection = Matrix4::OrthographicMatrix(0.f, (float)nNativeWidth, (float)nNativeHeight, 0.f, -1.f, 1.f); // left, right, bottom, top, near, far

	// Clear the window
	this->Clear();
}

void ott::Window::DropSystemPaths(const std::vector<std::string>& paths){
	if(userPathDropCallback){
		for(auto str = paths.cbegin(); str != paths.cend(); str++)
			(*userPathDropCallback)(*str);
	}
}

void ott::Window::SetWindowFocus(const bool& focused) {
	if (userWindowFocusCallback) {
		(*userWindowFocusCallback)(win.get(), focused ? GLFW_TRUE : GLFW_FALSE);
	}
}

/*
 * class ActiveWindows
 */

ott::ActiveWindows& ott::ActiveWindows::Instance(){
	static ActiveWindows instance;
	return instance;
}

void ott::ActiveWindows::Add(GLFWwindow* glfw, Window* win){
	listOfWindows[glfw] = win;
}

ott::Window* ott::ActiveWindows::Find(GLFWwindow* glfw){
	Window* retval = 0x0;
	auto win = listOfWindows.find(glfw);
	if (win != listOfWindows.end()) {
		retval = win->second;
	}
	return retval;
}

/*
 * Public functions
 */

uint32_t ott::GenerateVertexBufferObject(const std::vector<std::vector<float> >& elements, std::vector<size_t>& offsets) {
	// Generate Vertex VBO
	uint32_t vbo = 0;
	glCreateBuffers(1, &vbo);

	const size_t kNumberAttributes = elements.size();

	// Compute the total number of bytes
	size_t total_bytes = 0;
	std::vector<size_t> lengths(kNumberAttributes, 0);
	offsets = std::vector<size_t>(kNumberAttributes, 0);
	for (size_t i = 0; i < kNumberAttributes; i++) {
		lengths[i] = elements[i].size() * sizeof(float);
		offsets[i] = total_bytes;
		total_bytes += lengths[i];
	}

	// Bind buffer and reserve Vertex buffer memory
	glNamedBufferData(vbo, total_bytes, 0x0, GL_STATIC_DRAW);

	// Copy data to GPU
	for (size_t i = 0; i < kNumberAttributes; i++) {
		glNamedBufferSubData(vbo, (GLintptr)offsets[i], (GLsizeiptr)lengths[i], reinterpret_cast<const void*>(elements[i].data()));
	}

	return vbo;
}
