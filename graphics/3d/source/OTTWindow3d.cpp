#include "OTTWindow3d.hpp"

#include <graphics/geometry/OTTObject.hpp>
#include <math/OTTConstants.hpp>

#include <iostream>
#include <map>

ott::Window3d::~Window3d(){
}

ott::Shader3d* ott::Window3d::GetShader(const ShaderType& type) {
	return shaders->Get(type);
}

void ott::Window3d::DrawPixel(const float& x, const float& y, const float& z) {
	glBegin(GL_POINTS);
		glVertex3f(x, y, z);
	glEnd();
}

void ott::Window3d::DrawLine(
	const float& x1, const float& y1, const float& z1,
	const float& x2, const float& y2, const float& z2) 
{
	glBegin(GL_LINES);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	glEnd();
}

void ott::Window3d::DrawLine(const Vector3& p1, const Vector3& p2) {
	glBegin(GL_LINES);
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();
}

void ott::Window3d::DrawPolyline(const std::vector<Vector3>& points) {
	glBegin(GL_LINES);
	for (auto p = points.cbegin(); p != points.cend(); p++) // Draw the points
		glVertex3f((*p)[0], (*p)[1], (*p)[2]);
	glEnd();
}

void ott::Window3d::DrawPolygon(const std::vector<Vector3>& points) {
	glBegin(GL_LINES);
	for (auto p = points.cbegin(); p != points.cend(); p++) // Draw the sides
		glVertex3f((*p)[0], (*p)[1], (*p)[2]);
	glVertex3f(points[0][0], points[0][1], points[0][2]); // Close the polygon
	glEnd();
}

void ott::Window3d::DrawTexture(const GLuint& texture, const Vector2& p1, const Vector2& p2, const float& depth) {
	// Transform the input screen coordinates to NDC coordinates using the orthographic projection matrix.
	// input: x=(0 to W), y=(0 to H)
	// output: x=(-W/2 to W/2), y=(-H/2 to H/2)
	Vector4 p1NDC = mOrthoProjection.Transform(Vector4(p1[0] - nNativeWidth / 2.f, p1[1] - nNativeHeight / 2.f, 0.f, 1.f));
	Vector4 p2NDC = mOrthoProjection.Transform(Vector4(p2[0] - nNativeWidth / 2.f, p2[1] - nNativeHeight / 2.f, 0.f, 1.f));

	// Coordinates are now in NDC space.
	// Note that drawTextureNDC expects the bottom left and top right coordinates in NDC space
	// because transforming from screen to NDC space inverts the vertical axis.
	// The y components of the transformed vectors are thus flipped in the call to drawTextureNDC.
	this->DrawTextureNDC(texture, Vector2(p1NDC[0], p2NDC[1]), Vector2(p2NDC[0], p1NDC[1]), -1.f + 2.f * depth); 
}

void ott::Window3d::DrawTextureNDC(const GLuint& texture, const Vector2& p1, const Vector2& p2, const float& depth/*=-1.f*/) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0.f, 0.f); glVertex3f(p1[0], p1[1], depth); // bottom left
		glTexCoord2f(1.f, 0.f); glVertex3f(p2[0], p1[1], depth); // bottom right
		glTexCoord2f(1.f, 1.f); glVertex3f(p2[0], p2[1], depth); // top right
		glTexCoord2f(0.f, 1.f); glVertex3f(p1[0], p2[1], depth); // top left
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ott::Window3d::DrawVertexArray(const float* vertices, const std::vector<uint16_t>& indices) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, indices.data());
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ott::Window3d::DrawObject(const Object* obj) {
	// Bind VBOs for vertex data array and index array
	glBindBuffer(GL_ARRAY_BUFFER, obj->VertexVBO());
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->getIndexVBO());
		
	// Add vertex attribute pointers
	const PolygonContainer* polys = obj->ConstPolygons();
	for (size_t i = 0; i < polys->NumberOfVertexAttributes(); i++) {
		// Unfortunately we have to do this messy pointer cast with the offsets
		glVertexAttribPointer((GLuint)i, (GLint)polys->NumberOfAttributeElements(i), GL_FLOAT, GL_FALSE, 0, (const void*)polys->RawDataOffset(i));

		// Enable the attribute
		glEnableVertexAttribArray((GLuint)i);
	}

	// Draw the faces
	//glDrawElements(GL_TRIANGLES, (GLsizei)(polys->getNumberOfVertices()), GL_UNSIGNED_SHORT, 0x0);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(polys->NumberOfVertices()));

	for (size_t i = 0; i < polys->NumberOfVertexAttributes(); i++) {
		// Disable vertex attributes
		glDisableVertexAttribArray((GLuint)i);
	}
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ott::Window3d::DrawAxes() {
	this->SetDrawColor(colors::Red);
	this->DrawLine(0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
	this->SetDrawColor(colors::Green);
	this->DrawLine(0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
	this->SetDrawColor(colors::Blue);
	this->DrawLine(0.f, 0.f, 0.f, 0.f, 0.f, 1.f);
}

void ott::Window3d::EnableZDepth() {
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	b3dMode = true;
	this->Reshape3D(); // Update view matrix for 3d
}

void ott::Window3d::DisableZDepth() {
	glDisable(GL_DEPTH_TEST);
	b3dMode = false;
	this->Reshape(); // Update view matrix for 2d
}

void ott::Window3d::EnableCulling() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void ott::Window3d::DisableCulling() {
	glDisable(GL_CULL_FACE);
}

void ott::Window3d::EnableWireframeMode() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void ott::Window3d::DisableWireframeMode() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ott::Window3d::EnableShader(const ShaderType& type) {
	glUseProgram(shaders->Get(type)->Program());
}

void ott::Window3d::DisableShader() {
	glUseProgram(0);
}

void ott::Window3d::ResetModelviewMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Reset model-view matrix
}

void ott::Window3d::TranslateModelviewMatrix(const Vector3& pos) {
	glTranslatef(pos[0], pos[1], -pos[2]); // Translation
}

void ott::Window3d::RotateModelviewMatrix(const float& x, const float& y, const float& z) {
	glRotatef(x * constants::kRad2deg, 1.f, 0.f, 0.f); // Rotation about x-axis (pitch)
	glRotatef(y * constants::kRad2deg, 0.f, 1.f, 0.f); // Rotation about y-axis (yaw)
	glRotatef(z * constants::kRad2deg, 0.f, 0.f, 1.f); // Rotation about z-axis (roll)
}

void ott::Window3d::Reshape3D() {
	this->SetCurrent();

	this->UpdatePixelZoom();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Update viewport
	glViewport(0, 0, width, height);
	
	// Enable perspective projection with fovy, aspect, zNear and zFar
	gluPerspective(fFieldOfView, fNativeAspect, fNearPlane, fFarPlane);
	glMatrixMode(GL_MODELVIEW);

	// For 2d graphics on top of a 3d scene
	mOrthoProjection = Matrix4::OrthographicMatrix(0, (float)nNativeWidth, (float)nNativeHeight, 0.f, fNearPlane, fFarPlane); // left, right, bottom, top, near, far

	// Clear the window.
	this->Clear();
}

void ott::Window3d::OnUserReshape(){
	if(b3dMode){ // 3d
		this->Reshape3D();
	}
	else{ // 2d
		this->Reshape();
	}
}

void ott::Window3d::OnUserInitialize(){
	// Initialize default shaders
	shaders.reset(new shaders::ShaderList());
}

