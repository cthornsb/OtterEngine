#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Globals.hpp"
#include "GraphicsOpenGL.hpp"
#include "object.hpp"

std::map<int, Window*> listOfWindows;

Window *getCurrentWindow(){
	return listOfWindows[glutGetWindow()];
}

void handleMouse(int button, int state, int x, int y) {
	// Handle mouse clicks
	Window* currentWindow = getCurrentWindow();
	if(state == GLUT_UP)
		currentWindow->getMouse()->up(button);
	else if(state == GLUT_DOWN)
		currentWindow->getMouse()->down(button);
}

void handleMouseMovement(int x, int y) {
	// Handle active mouse movement (i.e. movement while holding a mouse button)
	//getCurrentWindow()->getMouse()->setPosition(x, y);
}

void handleMouseMovementPassive(int x, int y) {
	// Handle passive mouse movement (i.e. movement without holding a mouse button)
	getCurrentWindow()->getMouse()->setPosition(x, y);
}

/** Handle OpenGL keyboard presses.
  * @param key The keyboard character which was pressed.
  * @param x X coordinate of the mouse when the key was pressed (not used).
  * @param y Y coordinate of the mouse when the key was pressed (not used).
  */
void handleKeys(unsigned char key, int x, int y){
	Window *currentWindow = getCurrentWindow();
	if(key == 0x1B){ // Escape
		currentWindow->close();
		return;
	}
	currentWindow->getKeypress()->keyDown(key);
}

/** Handle OpenGL keyboard key releases.
  * @param key The keyboard character which was pressed.
  * @param x X coordinate of the mouse when the key was released (not used).
  * @param y Y coordinate of the mouse when the key was released (not used).
  */
void handleKeysUp(unsigned char key, int x, int y){
	getCurrentWindow()->getKeypress()->keyUp(key);
}

/** Handle OpenGL special key presses.
  * Redirect special keys to a more convenient format.
  * @param key The special key which was pressed
  * @param x X coordinate of the mouse when the key was pressed (not used)
  * @param y Y coordinate of the mouse when the key was pressed (not used)
  */
void handleSpecialKeys(int key, int x, int y){
	unsigned char ckey = 0x0;
	switch (key) {
		case GLUT_KEY_UP:
			ckey = 0x52;
			break;
		case GLUT_KEY_LEFT:
			ckey = 0x50;
			break;
		case GLUT_KEY_DOWN:
			ckey = 0x51;
			break;
		case GLUT_KEY_RIGHT:
			ckey = 0x4F;
			break;
		default:
			if (key >= GLUT_KEY_F1 && key <= GLUT_KEY_F12) { // Function keys
				ckey = 0xF0;
				ckey |= (key & 0x0F);
			}
			else { return; }
			break;
	}
	if(ckey)
		handleKeys(ckey, x, y);
}

/** Handle OpenGL special key releases.
  * Redirect special keys to match the ones used by SDL.
  * @param key The special key which was pressed
  * @param x X coordinate of the mouse when the key was released (not used)
  * @param y Y coordinate of the mouse when the key was released (not used)
  */
void handleSpecialKeysUp(int key, int x, int y){
	unsigned char ckey = 0x0;
	switch (key) {
		case GLUT_KEY_UP:
			ckey = 0x52;
			break;
		case GLUT_KEY_LEFT:
			ckey = 0x50;
			break;
		case GLUT_KEY_DOWN:
			ckey = 0x51;
			break;
		case GLUT_KEY_RIGHT:
			ckey = 0x4F;
			break;
		default:
			return;
	}
	if(ckey)
		handleKeysUp(ckey, x, y);
}

/** Handle window resizes.
  * @param width The new width of the window after the user has resized it.
  * @param height The new height of the window after the user has resized it.
  */
void reshapeScene2D(GLint width, GLint height){
	Window *currentWindow = getCurrentWindow();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int wprime = width;
	int hprime = height;
	float aspect = float(wprime)/hprime;

	if(aspect > currentWindow->getAspectRatio()){ // Wider window (height constrained)
		wprime = (int)(currentWindow->getAspectRatio() * hprime);
		glPointSize(float(hprime)/currentWindow->getHeight());
	}
	else{ // Taller window (width constrained)
		hprime = (int)(wprime / currentWindow->getAspectRatio());
		glPointSize(float(wprime)/currentWindow->getWidth());
	}

	glViewport(0, 0, wprime, hprime); // Update the viewport
	gluOrtho2D(0, currentWindow->getWidth(), currentWindow->getHeight(), 0);
	glMatrixMode(GL_MODELVIEW);

	// Update window size
	currentWindow->setWidth(wprime);
	currentWindow->setHeight(hprime);

	glutPostRedisplay();
	
	// Clear the window.
	currentWindow->clear();
}

void reshapeScene3D(GLint width, GLint height) {
	// Compute aspect ratio of the new window
	if (height == 0) height = 1;                // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset

	// Enable perspective projection with fovy, aspect, zNear and zFar
	gluPerspective(90.f, aspect, 0.1f, 10.0f);
	glMatrixMode(GL_MODELVIEW);

	// Clear the window.
	getCurrentWindow()->clear();
}

void displayFunction() {
	// This callback does nothing, but is required on Windows
}

/////////////////////////////////////////////////////////////////////
// class MouseState
/////////////////////////////////////////////////////////////////////

MouseState::MouseState() :
	count(0),
	posX(0),
	posY(0),
	dX(0),
	dY(0),
	lockPointer(false),
	states()
{
	reset();
}

bool MouseState::delta() {
	return (dX != 0 || dY != 0);
}

void MouseState::setPosition(const int& x, const int& y) { 
	static bool firstPosition = true;
	if (firstPosition) {
		firstPosition = false;
		Window* win = getCurrentWindow();
		glutWarpPointer(win->getWidth() / 2, win->getHeight() / 2);
	}
	if (!lockPointer) {
		dX = x - posX;
		dY = y - posY;
		posX = x;
		posY = y;
	}
	else{
		Window* win = getCurrentWindow();
		dX = x - win->getWidth() / 2;
		dY = y - win->getHeight() / 2;
		glutWarpPointer(win->getWidth() / 2, win->getHeight() / 2);
	}
}

bool MouseState::delta(int& deltaX, int& deltaY) {
	deltaX = dX;
	deltaY = dY;
	dX = 0;
	dY = 0;
	return (deltaX != 0 || deltaY != 0);
}

bool MouseState::poll(const int& button) {
	if (states[button]) {
		states[button] = false;
		return true;
	}
	return false;
}

void MouseState::down(const int& button) {
	if (!states[button]) {
		states[button] = true;
		count++;
	}
}

void MouseState::up(const int& button) {
	if (states[button]) {
		states[button] = false;
		count--;
	}
}

void MouseState::reset() {
	for (int i = 0; i < 3; i++)
		states[i] = false;
	count = 0;
}

/////////////////////////////////////////////////////////////////////
// class KeyStates
/////////////////////////////////////////////////////////////////////

KeyStates::KeyStates() : count(0), streamMode(false) {
	reset();
}

void KeyStates::enableStreamMode(){
	streamMode = true;
	reset();
}

void KeyStates::disableStreamMode(){
	streamMode = false;
	reset();
}

bool KeyStates::poll(const unsigned char &key){ 
	if(states[key]){
		states[key] = false;
		return true;
	}
	return false;
}

void KeyStates::keyDown(const unsigned char &key){
	if(!streamMode){
		if(!states[key]){
			states[key] = true;
			count++;
		}
	}
	else{
		buffer.push((char)key);
	}
}

void KeyStates::keyUp(const unsigned char &key){
	if(!streamMode){
		if(states[key]){
			states[key] = false;
			count--;
		}
	}
}

bool KeyStates::get(char& key){
	if(buffer.empty())
		return false;
	key = buffer.front();
	buffer.pop();
	return true;
}

void KeyStates::reset(){
	for(int i = 0; i < 256; i++)
		states[i] = false;
	while(!buffer.empty())
		buffer.pop();
	count = 0;
}

/////////////////////////////////////////////////////////////////////
// class Window
/////////////////////////////////////////////////////////////////////

Window::~Window(){
	close();
}

void Window::close(){
	glutDestroyWindow(winID);
	init = false;
}

bool Window::processEvents(){
	if(!status())
		return false;
	glutMainLoopEvent();
	return true;
}

Shader* Window::getShader(const ShaderType& type) {
	return shaders->get(type);
}

void Window::setScalingFactor(const int &scale){ 
	nMult = scale; 
	glutReshapeWindow(W*scale, H*scale);
}
	
void Window::setDrawColor(ColorRGB *color, const float &alpha/*=1*/){
	glColor4f(color->r, color->g, color->b, color->a);
}

void Window::setDrawColor(const ColorRGB &color, const float &alpha/*=1*/){
	glColor4f(color.r, color.g, color.b, color.a);
}

void Window::setCurrent(){
	glutSetWindow(winID);
}

void Window::clear(const ColorRGB &color/*=Colors::BLACK*/){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::drawPixel(const int &x, const int &y){
	glBegin(GL_POINTS);
		glVertex2i(x, y);
	glEnd();
}

void Window::drawPixel(const int *x, const int *y, const size_t &N){
	for(size_t i = 0; i < N; i++) // Draw N pixels
		drawPixel(x[i], y[i]);
}

void Window::drawPixel(const float& x, const float& y, const float& z) {
	glBegin(GL_POINTS);
		glVertex3f(x, y, z);
	glEnd();
}

void Window::drawLine(const int &x1, const int &y1, const int &x2, const int &y2){
	glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
	glEnd();
}

void Window::drawLine(const int *x, const int *y, const size_t &N){
	if(N < 2) // Nothing to draw
		return;
	for(size_t i = 0; i < N-1; i++)
		drawLine(x[i], y[i], x[i+1], y[i+1]);
}

void Window::drawLine(
	const float& x1, const float& y1, const float& z1,
	const float& x2, const float& y2, const float& z2) 
{
	glBegin(GL_LINES);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	glEnd();
}

void Window::drawLine(const Vector3& p1, const Vector3& p2) {
	glBegin(GL_LINES);
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();
}

void Window::drawRectangle(const int &x1, const int &y1, const int &x2, const int &y2){
	drawLine(x1, y1, x2, y1); // Top
	drawLine(x2, y1, x2, y2); // Right
	drawLine(x2, y2, x1, y2); // Bottom
	drawLine(x1, y2, x1, y1); // Left
}

void Window::drawTexture(const unsigned int& texture, const int& x1, const int& y1, const int& x2, const int& y2) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(x1, y1);
		glTexCoord2i(1, 0); glVertex2i(x2, y1);
		glTexCoord2i(1, 1); glVertex2i(x2, y2);
		glTexCoord2i(0, 1); glVertex2i(x1, y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Window::drawVertexArray(const float* vertices, const std::vector<unsigned short>& indices) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, indices.data());
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Window::drawObject(const object* obj) {
	// Bind VBOs for vertex data array and index array
	glBindBuffer(GL_ARRAY_BUFFER, obj->getVertexVBO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->getIndexVBO());
		
	// do same as vertex array except pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)obj->getPolygons()->getVertexDataOffset());
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void*)obj->getPolygons()->getNormalDataOffset());
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const void*)obj->getPolygons()->getColorDataOffset());
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (const void*)obj->getPolygons()->getTextureDataOffset());

	// Enable vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	// Draw the faces
	glDrawElements(GL_TRIANGLES, (GLsizei)(3 * obj->getNumberOfPolygons()), GL_UNSIGNED_SHORT, 0x0);

	// Disable vertex attributes
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Window::render(){
	glFlush();
}

bool Window::status(){
	return init;
}

void Window::initialize(const std::string& name/*="OpenGL"*/){
	if(init) return;

	// Dummy command line arguments
	int dummyArgc = 1;

	// Open the graphics window
	static bool firstInit = true;
	if(firstInit){ // Initialize GLUT
		glutInit(&dummyArgc, NULL);
	}
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(W*nMult, H*nMult);
	glutInitWindowPosition(100, 100);
	winID = glutCreateWindow(name.c_str());
	listOfWindows[winID] = this;

	// Set window size handler
	glutReshapeFunc(reshapeScene2D);

	// Update the viewport
	reshapeScene2D(W * nMult, H * nMult);

	// Set the display function callback (required for Windows)
	glutDisplayFunc(displayFunction);

	if (firstInit) { // Initialize GLEW
		GLenum err = glewInit(); // Initialize GLEW
		if (err != GLEW_OK) {
			std::cout << " [ERROR] Failed to initialize GLEW!" << std::endl;
		}
		firstInit = false;
	}

	// Initialize default shaders
	shaders.reset(new DefaultShaders::ShaderList());

	init = true;
}

void Window::setKeyboardStreamMode(){
	// Enable keyboard repeat
	glutIgnoreKeyRepeat(0);
	keys.enableStreamMode();
}

void Window::setKeyboardToggleMode(){
	// Disable keyboard repeat
	glutIgnoreKeyRepeat(1);
	keys.disableStreamMode();
}

void Window::setupKeyboardHandler(){
	// Set keyboard handler
	glutKeyboardFunc(handleKeys);
	glutSpecialFunc(handleSpecialKeys);

	// Keyboard up handler
	glutKeyboardUpFunc(handleKeysUp);
	glutSpecialUpFunc(handleSpecialKeysUp);

	// Set keyboard keys to behave as button toggles
	setKeyboardToggleMode();
}

void Window::setupMouseHandler() {
	// Set mouse handler
	glutMouseFunc(handleMouse);

	// Set active movement handler
	glutMotionFunc(handleMouseMovement);

	// Set passive movement handler
	glutPassiveMotionFunc(handleMouseMovementPassive);
}

void Window::enableAlphaBlending() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::enableZDepth() {
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glutReshapeFunc(reshapeScene3D);
	reshapeScene3D(W * nMult, H * nMult);
}

void Window::disableZDepth() {
	glDisable(GL_DEPTH_TEST);
	glutReshapeFunc(reshapeScene2D);
	reshapeScene2D(W * nMult, H * nMult);
}

void Window::enableCulling() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Window::disableCulling() {
	glDisable(GL_CULL_FACE);
}

void Window::enableWireframeMode() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Window::disableWireframeMode() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::enableShader(const ShaderType& type) {
	glUseProgram(shaders->get(type)->getProgram());
}

void Window::enableShader(const Shader* shdr) {
	glUseProgram(shdr->getProgram());
}

void Window::disableShader() {
	glUseProgram(0);
}

void Window::resetModelviewMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Reset model-view matrix
}

void Window::translateModelviewMatrix(const Vector3& pos) {
	glTranslatef(pos[0], pos[1], -pos[2]); // Translation
}

void Window::rotateModelviewMatrix(const float& x, const float& y, const float& z) {
	glRotatef(x * rad2deg, 1.f, 0.f, 0.f); // Rotation about x-axis (pitch)
	glRotatef(y * rad2deg, 0.f, 1.f, 0.f); // Rotation about y-axis (yaw)
	glRotatef(z * rad2deg, 0.f, 0.f, 1.f); // Rotation about z-axis (roll)
}

void Window::paintGL(){
	this->render();
}

void Window::initializeGL(){
	this->initialize();
}

void Window::resizeGL(int width, int height){
	reshapeScene2D(width, height);
}
