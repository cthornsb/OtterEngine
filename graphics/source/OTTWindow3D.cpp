#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Globals.hpp"
#include "OTTWindow3D.hpp"
#include "object.hpp"

OTTWindow3D::~OTTWindow3D(){
}

OTTShader* OTTWindow3D::getShader(const ShaderType& type) {
	return shaders->get(type);
}

void OTTWindow3D::drawPixel(const int &x, const int &y){
	glBegin(GL_POINTS);
		glVertex2i(x, y);
	glEnd();
}

void OTTWindow3D::drawPixel(const int *x, const int *y, const size_t &N){
	for(size_t i = 0; i < N; i++) // Draw N pixels
		drawPixel(x[i], y[i]);
}

void OTTWindow3D::drawPixel(const float& x, const float& y, const float& z) {
	glBegin(GL_POINTS);
		glVertex3f(x, y, z);
	glEnd();
}

void OTTWindow3D::drawLine(const int &x1, const int &y1, const int &x2, const int &y2){
	glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
	glEnd();
}

void OTTWindow3D::drawLine(const int *x, const int *y, const size_t &N){
	if(N < 2) // Nothing to draw
		return;
	for(size_t i = 0; i < N-1; i++)
		drawLine(x[i], y[i], x[i+1], y[i+1]);
}

void OTTWindow3D::drawLine(
	const float& x1, const float& y1, const float& z1,
	const float& x2, const float& y2, const float& z2) 
{
	glBegin(GL_LINES);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	glEnd();
}

void OTTWindow3D::drawLine(const Vector3& p1, const Vector3& p2) {
	glBegin(GL_LINES);
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();
}

void OTTWindow3D::drawRectangle(const int &x1, const int &y1, const int &x2, const int &y2){
	drawLine(x1, y1, x2, y1); // Top
	drawLine(x2, y1, x2, y2); // Right
	drawLine(x2, y2, x1, y2); // Bottom
	drawLine(x1, y2, x1, y1); // Left
}

void OTTWindow3D::drawTexture(const unsigned int& texture, const int& x1, const int& y1, const int& x2, const int& y2) {
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

void OTTWindow3D::drawVertexArray(const float* vertices, const std::vector<unsigned short>& indices) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, indices.data());
	glDisableClientState(GL_VERTEX_ARRAY);
}

void OTTWindow3D::drawObject(const object* obj) {
	// Bind VBOs for vertex data array and index array
	glBindBuffer(GL_ARRAY_BUFFER, obj->getVertexVBO());
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->getIndexVBO());
		
	// Add vertex attribute pointers
	const PolygonContainer* polys = obj->getConstPolygonContainer();
	for (size_t i = 0; i < polys->getNumberOfVertexAttributes(); i++) {
		// Unfortunately we have to do this messy pointer cast with the offsets
		glVertexAttribPointer((GLuint)i, (GLint)polys->getNumberOfAttributeElements(i), GL_FLOAT, GL_FALSE, 0, (const void*)polys->getRawDataOffset(i));

		// Enable the attribute
		glEnableVertexAttribArray((GLuint)i);
	}

	// Draw the faces
	//glDrawElements(GL_TRIANGLES, (GLsizei)(polys->getNumberOfVertices()), GL_UNSIGNED_SHORT, 0x0);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(polys->getNumberOfVertices()));

	for (size_t i = 0; i < polys->getNumberOfVertexAttributes(); i++) {
		// Disable vertex attributes
		glDisableVertexAttribArray((GLuint)i);
	}
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OTTWindow3D::enableAlphaBlending() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OTTWindow3D::enableZDepth() {
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	b3dMode = true;
	reshape3D(); // Update view matrix for 3d
}

void OTTWindow3D::disableZDepth() {
	glDisable(GL_DEPTH_TEST);
	b3dMode = false;
	reshape(); // Update view matrix for 2d
}

void OTTWindow3D::enableCulling() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void OTTWindow3D::disableCulling() {
	glDisable(GL_CULL_FACE);
}

void OTTWindow3D::enableWireframeMode() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void OTTWindow3D::disableWireframeMode() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OTTWindow3D::enableShader(const ShaderType& type) {
	glUseProgram(shaders->get(type)->getProgram());
}

void OTTWindow3D::disableShader() {
	glUseProgram(0);
}

void OTTWindow3D::resetModelviewMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Reset model-view matrix
}

void OTTWindow3D::translateModelviewMatrix(const Vector3& pos) {
	glTranslatef(pos[0], pos[1], -pos[2]); // Translation
}

void OTTWindow3D::rotateModelviewMatrix(const float& x, const float& y, const float& z) {
	glRotatef(x * rad2deg, 1.f, 0.f, 0.f); // Rotation about x-axis (pitch)
	glRotatef(y * rad2deg, 0.f, 1.f, 0.f); // Rotation about y-axis (yaw)
	glRotatef(z * rad2deg, 0.f, 0.f, 1.f); // Rotation about z-axis (roll)
}

void OTTWindow3D::reshape3D() {
	setCurrent();
	
	updatePixelZoom();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Update viewport
	glViewport(0, 0, width, height);
	
	// Enable perspective projection with fovy, aspect, zNear and zFar
	gluPerspective(fFieldOfView, fNativeAspect, fNearPlane, fFarPlane);
	glMatrixMode(GL_MODELVIEW);

	// Clear the window.
	clear();
}

void OTTWindow3D::onUserReshape(){
	if(b3dMode){ // 3d
		reshape3D();
	}
	else{ // 2d
		reshape();
	}
}

void OTTWindow3D::onUserInitialize(){
	if (bFirstInit) { // Initialize GLEW
		GLenum err = glewInit(); // Initialize GLEW
		if (err != GLEW_OK) {
			std::cout << " [ERROR] Failed to initialize GLEW!" << std::endl;
		}
	}

	// Initialize default shaders
	shaders.reset(new OTTDefaultShaders::ShaderList());
}

