#include "OTTScene.hpp"

#include <graphics/3d/OTTCamera.hpp>
#include <graphics/geometry/OTTObject.hpp>

#include <iostream>

#ifndef WIN32
#include <unistd.h>
#endif // ifndef WIN32

//#include "scene.hpp"
//#include "camera.hpp"
//#include "object.hpp"
//#include "OTTWindow3D.hpp"
//#include "OTTShader.hpp"

#ifdef SOFTWARE_RENDERER
#define SCREEN_XLIMIT 1.0 ///< Set the horizontal clipping border as a fraction of the total screen width
#define SCREEN_YLIMIT 1.0 ///< Set the vertical clipping border as a fraction of the total screen height
#endif // ifdef SOFTWARE_RENDERER

ott::Scene::Scene() : 
	FrameTimer(),
	drawOrigin(false), 
	drawDepthMap(false),
	isRunning(true), 
	cam(0x0),
#ifndef SOFTWARE_RENDERER
	window(new Window3d(640, 480, 1)),
#else
	window(new Window(640, 480, 1)),
	screenWidthPixels(640), 
	screenHeightPixels(480), 
	minPixelsX(0), 
	minPixelsY(0),
	maxPixelsX(640), 
	maxPixelsY(480),
	mode(drawMode::WIREFRAME),
#endif // ifdef SOFTWARE_RENDERER
	worldLight(),
	objects(),
	lights()
#ifdef SOFTWARE_RENDERER
	,polygonsToDraw()
#endif // ifdef SOFTWARE_RENDERER
{ 
	this->Initialize();
}

ott::Scene::Scene(Camera *cam_) : 
	Scene()
{ 
	this->SetCamera(cam_);
}

ott::Scene::~Scene(){
}

void ott::Scene::Initialize(){
	// Initialize the window
	window->Initialize("Render3d");

#ifndef SOFTWARE_RENDERER
	// Switch to OpenGL renderer mode
	window->EnableZDepth();
	window->EnableCulling();
	window->EnableAlphaBlending();
#else
	// Set the pixel coordinate bounds
	minPixelsX = (int32_t)(screenWidthPixels*(1-SCREEN_YLIMIT)/2);
	maxPixelsX = (int32_t)(screenWidthPixels-minPixelsX);
	minPixelsY = (int32_t)(screenHeightPixels*(1-SCREEN_YLIMIT)/2);
	maxPixelsY = (int32_t)(screenHeightPixels-minPixelsX);
#endif // ifdef SOFTWARE_RENDERER

	// Add the world light to the list of light sources
	this->AddLight(&worldLight);
}

void ott::Scene::EnableVSync() {
	if (!window) {
		return;
	}
	window->EnableVSync();
	this->DisableFramerateCap(); // Disable built-in frame timer
}

void ott::Scene::DisableVSync(const double& fps/*=60.0*/) {
	if (!window) {
		return;
	}
	window->DisableVSync();
	this->SetFramerateCap(fps); // Update the frame timer with the new target framerate
}

void ott::Scene::AddObject(Object* obj) { 
	obj->Build(); // Construct the object
	if (!obj->GetShader()) { // Set default shader
		obj->SetShader(window->GetShader(ShaderType::Default));
	}
	objects.push_back(obj); // Add it to the list of objects to draw
}

void ott::Scene::Clear(const ColorRGB &color/*=colors::Black*/){
	window->Clear(color);
}

#ifndef SOFTWARE_RENDERER
bool ott::Scene::DrawOpenGL() {
	// Update frame timer
	this->Update();

	// Clear the screen with a color
	this->Clear(colors::Black);

	// Update projection and camera view transformation matrices
	Matrix4* proj = cam->ProjectionMatrix();
	Matrix4* view = cam->ViewMatrix();

	// Draw the origin
	if (drawOrigin) { 
		Matrix4 mvp = Matrix4::Concatenate(proj, view, &constants::kIdentityMatrix4);
		window->EnableShader(ShaderType::Default);
		window->GetShader(ShaderType::Default)->SetMatrix4("MVP", mvp);
		window->DrawAxes();
		window->DisableShader();
	}

	// Draw all objects
	for (auto obj = objects.cbegin(); obj != objects.cend(); obj++) {
		(*obj)->Draw(window.get(), proj, view);
	}

	//return render();
	if (window->ProcessEvents() == false) { // Check for events
		isRunning = false;
		return false;
	}
	return true;
}
#else
bool ott::Scene::update(){
	// Clear the vector of triangles to draw
	polygonsToDraw.clear();

	// Clear the screen with a color
	clear(Colors::BLACK);
	
	// Draw the 3d geometry
	for(auto obj = objects.cbegin(); obj != objects.cend(); obj++)
		processObject(*obj);

	// Compute vertex lighting
	for (std::vector<pixelTriplet>::iterator triplet = polygonsToDraw.begin(); triplet != polygonsToDraw.end(); triplet++) {
		for (std::vector<lightSource*>::iterator light = lights.begin(); light != lights.end(); light++) { // Over all lights in the scene
			if (!(*light)->isEnabled()) // Light disabled
				continue;
			triplet->computeVertexLighting(*light);
		}
		triplet->finalize();
	}

	if (mode == drawMode::RENDER) { // Compute pixel z-depth
		/*buffer.reset();
		int32_t x0, x1;
		float realy;
		const float farDistance = 10.f;
		for (std::vector<pixelTriplet>::iterator triplet = polygonsToDraw.begin(); triplet != polygonsToDraw.end(); triplet++) {
			for (int32_t scanline = 0; scanline < maxPixelsY; scanline++) {
				realy = (-2.f * scanline) / screenHeightPixels + 1;
				if (triplet->getHorizontalLimits(scanline, x0, x1)) { // Rasterization of triangle
					// TODO fix object vertices being rendered multiple times
					for (int32_t x = x0; x <= x1; x++) { // Over every pixel in the row
						float depth = triplet->getZDepth((2.f * x) / screenWidthPixels - 1, realy);
						if (depth < 0 || depth > farDistance)
							continue;
						buffer.set(x, scanline, depth, &(*triplet));
					}
				}
			}
		}
		// Draw the frame buffer
		float depth = 0;
		const pixelTriplet* trip = 0x0;
		for (uint16_t y = 0; y < maxPixelsY; y++) {
			realy = (-2.f * y) / screenHeightPixels + 1;
			for (uint16_t x = 0; x < maxPixelsX; x++) {
				if(!(trip = buffer.getTriangle(x, y))) // Blank pixel
					continue;
				depth = buffer.getDepth(x, y);
				if (!drawDepthMap) {
					if (!trip->p0)
						continue;
					window->setDrawColor(trip->getLighting((2.f * x) / screenWidthPixels - 1, realy));
				}
				else
					window->setDrawColor(ColorRGB::heatMap(depth, farDistance));
				window->drawPixel(x, y);
			}
		}
	}
	else if (mode == drawMode::SOLID) {
		int32_t x0, x1;
		const float farDistance = 10.f;
		for (std::vector<pixelTriplet>::const_iterator triplet = polygonsToDraw.cbegin(); triplet != polygonsToDraw.cend(); triplet++) { // Over all polygons to draw
			for (int32_t scanline = triplet->p0->pY; scanline <= triplet->p2->pY; scanline++) {
				if (triplet->getHorizontalLimits(scanline, x0, x1)) { // Rasterization of triangle
					// Draw the triangle face
					window->setDrawColor(Colors::WHITE);
					window->drawLine(x0, scanline, x1, scanline);

					// Draw the edges of the triangle
					drawTriangle(*triplet, Colors::BLACK);
					window->drawPixel(x0, scanline);
					window->drawPixel(x1, scanline);
				}
			}
		}*/
	}
	else {
		// Draw polygons
		for (auto triplet = polygonsToDraw.cbegin(); triplet != polygonsToDraw.cend(); triplet++) {
			// Draw the triangle to the screen
			drawTriangle(*triplet, Colors::WHITE);
		}
	}

	if (drawNorm) { // Draw the surface normal vector
		for (auto triplet = polygonsToDraw.cbegin(); triplet != polygonsToDraw.cend(); triplet++)
			drawVector(triplet->tri->getCenterPoint(), triplet->tri->getNormal(), Colors::RED, 0.25);
	}

	if(drawOrigin){ // Draw the origin
		drawVector(Vector3(0, 0, 0), Vector3(1, 0, 0), Colors::RED);
		drawVector(Vector3(0, 0, 0), Vector3(0, 1, 0), Colors::GREEN);
		drawVector(Vector3(0, 0, 0), Vector3(0, 0, 1), Colors::BLUE);
	}

	return render();
}
#endif // ifndef SOFTWARE_RENDERER

bool ott::Scene::Render() {
	// Update the screen	
	if (window->ProcessEvents() == false) { // Check for events
		isRunning = false;
		return false;
	}
	window->SetCurrent();
	window->Render();
	return true;
}

ott::Keyboard* ott::Scene::GetKeyboard(){
	return window->GetKeyboard();
}

ott::Mouse* ott::Scene::GetMouse(){
	return window->GetMouse();
}

void ott::Scene::SetCamera(Camera *cam_){ 
	cam = cam_; 
	cam->SetAspectRatio(window->AspectRatio());	
	cam->SetPerspectiveProjection(0.1f, 100.f); // Setup the camera projection matrix
}

#ifdef SOFTWARE_RENDERER
void ott::Scene::processPolygons(object* obj) {
	/*if (!obj->isVisible()) { // Check if the object is on screen
		return;
	}*/
	// Render all vertices
	obj->renderAllVertices(cam);
	// Process all visible polygons
	std::vector<triangle>* polys = obj->getPolygonVector();
	for (std::vector<triangle>::iterator iter = polys->begin(); iter != polys->end(); iter++) {
		// Do backface culling
		if (mode != drawMode::WIREFRAME && !cam->checkCulling(obj->getPosition(), (*iter))) // The triangle is facing away from the camera
			continue;

		// TODO check for triangles outside of view before rendering
		//if (!cam->checkVisible())
			//continue;

		// Render the triangle by converting its projection on the camera's viewing plane into pixel coordinates
		pixelTriplet pixels(&(*iter));
		//if (!cam->render(pixels)) // All vertices are behind camera
		//	continue;

		// Convert to pixel coordinates
		// (0, 0) is at the top-left of the screen
		if (!convertToPixelSpace(pixels)) // Check if the triangle is on the screen
			continue;

		// Rasterize the triangle
		if (!pixels.sortVertical(maxPixelsY))
			continue;

		// Add the triangle to the drawing list
		polygonsToDraw.push_back(pixels);
	}
}

void ott::Scene::processObject(object *obj){
	// Compute all parent polygons
	processPolygons(obj);
	for (auto child = obj->beginChildren(); child != obj->endChildren(); child++) { // And for all the object's children
		processPolygons((*child));
	}
}

void ott::Scene::convertToScreenSpace(const int32_t& px, const int32_t& py, float& x, float& y) const {
	x = (2.f * px) / screenWidthPixels - 1;
	y = (-2.f * py) / screenHeightPixels + 1;
}

bool ott::Scene::checkScreenSpace(const float& x, const float& y) const {
	return ((x >= -SCREEN_XLIMIT && x <= SCREEN_XLIMIT) && (y >= -SCREEN_YLIMIT && y <= SCREEN_YLIMIT));
}

bool ott::Scene::convertToPixelSpace(const float& x, const float& y, int32_t &px, int32_t &py) const {
	px = (int32_t)(screenWidthPixels*((x + 1)/2));
	py = (int32_t)(screenHeightPixels*(1 - (y + 1)/2));
	return checkScreenSpace(x, y);
}

bool ott::Scene::convertToPixelSpace(pixelTriplet &coords) const {
	bool retval = false;
	for (size_t i = 0; i < 3; i++) {
		coords.set(i, (int32_t)(screenWidthPixels * ((coords[i]->sX + 1) / 2)),
		              (int32_t)(screenHeightPixels * (1 - (coords[i]->sY + 1) / 2)));
		retval |= checkScreenSpace(coords[i]->sX, coords[i]->sY);
	}
	return retval;
}
#endif // ifndef SOFTWARE_RENDERER

void ott::Scene::DrawPoint(const Vector3 &point, const ColorRGB &color){
#ifndef SOFTWARE_RENDERER
	window->DrawPixel(point[0], point[1], point[2]);
#else
	float cmX, cmY;
	if(cam->projectPoint(point, cmX, cmY)){
		int32_t cmpX, cmpY;
	
		// Convert the screen-space coordinates to pixel-space
		if(!convertToPixelSpace(cmX, cmY, cmpX, cmpY)) // Check if the point is on the screen
			return;
		
		// Draw the normal vector
		window->setDrawColor(color);
		window->drawPixel(cmpX, cmpY);
	}
#endif // ifndef SOFTWARE_RENDERER
}

void ott::Scene::DrawVector(const Vector3 &start, const Vector3 &direction, const ColorRGB &color, const float &length/*=1*/){
#ifndef SOFTWARE_RENDERER
	window->DrawLine(start, start + (direction * length));
#else
	// Compute the end point
	Vector3 P = start + (direction * length);

	// Draw the triangle normals
	float cmX0, cmY0;
	float cmX1, cmY1;
	if(cam->projectPoint(start, cmX0, cmY0) && cam->projectPoint(P, cmX1, cmY1)){
		int32_t cmpX0, cmpY0;
		int32_t cmpX1, cmpY1;
		
		bool os1 = checkScreenSpace(cmX0, cmY0);
		bool os2 = checkScreenSpace(cmX1, cmY1);
		
		// Check if the line is on the screen
		if(!os1 && !os2) // The line is completely off the screen
			return;
		/*else if(!os1){ // (cmX0, cmY0) is off screen
			Vector3 p(cmX0-cmX1, cmY0-cmY1);
			double xcross, ycross;
			double slope = 0;//p.y/p.x;
			if(p.x >= 0) // Positive X
				ycross = cmY0+(1-cmX0)*slope;
			else // Negative X
				ycross = cmY0-(1+cmX0)*slope;
			if(p.y >= 0) // Positive Y
				xcross = (1-cmY0)/slope + cmX0;
			else // Negative Y
				xcross = -(1+cmY0)/slope + cmX0;
			
			if(xcross >= -1 && xcross <= 1) // Top and bottom
				cmX0 = xcross;
			else if(ycross >= -1 && ycross <= 1) // Left and right
				cmY0 = ycross;
		}*/
		/*else if(!os2){ // (cmX1, cmY1) is off screen
			Vector3 p(cmX1-cmX0, cmY1-cmY0);
			double xcross, ycross;
			double slope = -(cmY1-cmY0)/(cmX1-cmX0);
			if(p.x >= 0) // Positive X
				ycross = cmY0+(1-cmX0)*slope;
			else // Negative X
				ycross = cmY0-(1+cmX0)*slope;
			if(p.y >= 0) // Positive Y
				xcross = (1-cmY0)/slope + cmX0;
			else // Negative Y
				xcross = -(1+cmY0)/slope + cmX0;
			
			if(xcross >= -1 && xcross <= 1) // Top and bottom
				cmX1 = xcross;
			else if(ycross >= -1 && ycross <= 1) // Left and right
				cmY1 = ycross;
		}*/

		// Convert the screen-space coordinates to pixel-space
		convertToPixelSpace(cmX0, cmY0, cmpX0, cmpY0);
		convertToPixelSpace(cmX1, cmY1, cmpX1, cmpY1);
		
		// Draw the normal vector
		window->setDrawColor(color);
		window->drawLine(cmpX0, cmpY0, cmpX1, cmpY1);
	}
#endif // ifndef SOFTWARE_RENDERER
}

void ott::Scene::DrawRay(const Ray_t &proj, const ColorRGB &color, const float &length/*=1*/){
	this->DrawVector(proj.pos, proj.dir, color, length);
}

#ifdef SOFTWARE_RENDERER
void ott::Scene::drawTriangle(const pixelTriplet &coords, const ColorRGB &color){
	window->setDrawColor(color);
	for(size_t i = 0; i < 2; i++)
		window->drawLine(coords[i]->pX, coords[i]->pY, coords[i+1]->pX, coords[i+1]->pY);
	window->drawLine(coords[2]->pX, coords[2]->pY, coords[0]->pX, coords[0]->pY);
}

void ott::Scene::drawFilledTriangle(const pixelTriplet &coords, const ColorRGB &color){
	// Set the fill color
	window->setDrawColor(color);

	int32_t x0, x1;
	for(int32_t scanline = coords[0]->pY; scanline <= coords[2]->pY; scanline++){
		if (!coords.getHorizontalLimits(scanline, x0, x1))
			continue;

		// Draw the scanline
		window->drawLine(x0, scanline, x1, scanline);
	}
}
#endif // ifndef SOFTWARE_RENDERER
