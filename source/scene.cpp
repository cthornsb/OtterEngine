#include <iostream>
#include <thread>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "scene.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "Graphics.hpp"

#define SCREEN_XLIMIT 1.0 ///< Set the horizontal clipping border as a fraction of the total screen width
#define SCREEN_YLIMIT 1.0 ///< Set the vertical clipping border as a fraction of the total screen height

scene::scene() : 
	framerate(0), 
	framerateCap(60),
	totalRenderTime(0),
	framePeriod(16667),
	frameCount(0),
	drawNorm(false), 
	drawOrigin(false), 
	drawDepthMap(false),
	isRunning(true), 
	screenWidthPixels(640), 
	screenHeightPixels(480), 
	minPixelsX(0), 
	minPixelsY(0),
	maxPixelsX(640), 
	maxPixelsY(480),
	mode(drawMode::WIREFRAME),
	buffer(640, 480),
	timeOfInitialization(hclock::now()),
	timeOfLastUpdate(),
	cam(0x0),
	window(new Window(screenWidthPixels, screenHeightPixels, 1)),
	worldLight(),
	objects(),
	lights(),
	polygonsToDraw()
{ 
	initialize();
}

scene::scene(camera *cam_) : scene()
{ 
	initialize();
	setCamera(cam_);
}

scene::~scene(){
}

void scene::initialize(){
	// Setup the window
	window->initialize("Render3d");
	window->setupMouseHandler(); // Set mouse support
	window->setupKeyboardHandler(); // Set keyboard support
	window->getMouse()->setLockPointer();
	
	// Set the pixel coordinate bounds
	minPixelsX = (int)(screenWidthPixels*(1-SCREEN_YLIMIT)/2);
	maxPixelsX = (int)(screenWidthPixels-minPixelsX);
	minPixelsY = (int)(screenHeightPixels*(1-SCREEN_YLIMIT)/2);
	maxPixelsY = (int)(screenHeightPixels-minPixelsX);
}

void scene::addObject(object* obj) { 
	obj->build(); // Construct the object
	objects.push_back(obj); // Add it to the list of objects to draw
}

void scene::clear(const ColorRGB &color/*=Colors::BLACK*/){
	window->clear(color);
}

bool scene::update(){
	// Update the timer
	timeOfLastUpdate = hclock::now();
	
	// Clear the vector of triangles to draw
	polygonsToDraw.clear();

	// Clear the screen with a color
	clear(Colors::BLACK);
	
	// Draw the 3d geometry
	for(auto obj = objects.cbegin(); obj != objects.cend(); obj++)
		processObject(*obj);

	if (mode == drawMode::RENDER) { // Compute pixel z-depth
		int x0, x1;
		float realy;
		const float farDistance = 10.f;
		for (auto triplet = polygonsToDraw.cbegin(); triplet != polygonsToDraw.cend(); triplet++) {
			for (int scanline = 0; scanline < maxPixelsY; scanline++) {
				realy = (-2.f * scanline) / screenHeightPixels + 1;
				if (triplet->getHorizontalLimits(scanline, x0, x1)) { // Rasterization of triangle
					// TODO fix object vertices being rendered multiple times
					for (int x = x0; x <= x1; x++) { // Over every pixel in the row
						float depth = triplet->calc.getZ((2.f * x0) / screenWidthPixels - 1, realy);
						if (depth > farDistance)
							continue;
						if (!drawDepthMap)
							window->setDrawColor(worldLight.getColor(triplet->tri));
						else
							window->setDrawColor(ColorRGB::heatMap(depth, farDistance));
						//window->drawLine(x0, scanline, x1, scanline);
						//buffer.set(x, scanline, depth, &(*triplet));
						window->drawPixel(x, scanline);
					}
				}
			}
		}
	}
	else if (mode == drawMode::SOLID) {
		int x0, x1;
		const float farDistance = 10.f;
		for (std::vector<pixelTriplet>::const_iterator triplet = polygonsToDraw.cbegin(); triplet != polygonsToDraw.cend(); triplet++) { // Over all polygons to draw
			for (int scanline = triplet->pY[0]; scanline <= triplet->pY[2]; scanline++) {
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
		}
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
			drawVector(triplet->getCenterPoint(), triplet->tri->norm, Colors::RED, 0.25);
	}

	if(drawOrigin){ // Draw the origin
		drawVector(vector3(0, 0, 0), vector3(1, 0, 0), Colors::RED);
		drawVector(vector3(0, 0, 0), vector3(0, 1, 0), Colors::GREEN);
		drawVector(vector3(0, 0, 0), vector3(0, 0, 1), Colors::BLUE);
	}

	//if (!polygonsToDraw.empty())
		//drawVector(vector3(0, 0, 0), polygonsToDraw.back().tri->p, Colors::RED);

	// Update the screen
	if(!window->processEvents()){ // Check for events
		isRunning = false;
		return false;
	}
	window->render();

	return true;
}

/** Sync the frame timer to the requested framerate
  */
float scene::sync() {
	// Get the time since the frame started
	long long frameTime = std::chrono::duration_cast<std::chrono::microseconds>(hclock::now() - timeOfLastUpdate).count(); // in microseconds
	totalRenderTime += frameTime;

	// Update the total render time and the instantaneous framerate
	if (++frameCount >= (long long)framerateCap) { // Compute framerate (every second)
		framerate = frameCount / (totalRenderTime * 1.0E-6);
		totalRenderTime = 0;
		frameCount = 0;
	}

	// Cap the framerate by sleeping
	if (framerateCap > 0) {
		long long timeToSleep = framePeriod - frameTime; // microseconds
		if (timeToSleep > 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(timeToSleep));
			totalRenderTime += timeToSleep;
		}
	}

	// Return the time elapsed since last call to sync()
	return std::chrono::duration<float>(hclock::now() - timeOfLastUpdate).count(); // in seconds
}

/** Get the total time elapsed since the scene was initialized (in seconds)
  */
double scene::getTimeElapsed() const { 
	return std::chrono::duration_cast<std::chrono::duration<double>>(hclock::now() - timeOfInitialization).count();
}

KeyStates* scene::getKeypress(){
	return window->getKeypress();
}

MouseState* scene::getMouse(){
	return window->getMouse();
}

void scene::setCamera(camera *cam_){ 
	cam = cam_; 
	cam->setAspectRatio(float(screenWidthPixels)/screenHeightPixels);
}

void scene::processPolygons(object* obj) {
	const std::vector<triangle>* polys = obj->getPolygons();
	const vector3* offset = obj->getConstPositionPointer();
	for (auto iter = polys->cbegin(); iter != polys->cend(); iter++) {
		// Do backface culling
		if (mode != drawMode::WIREFRAME && !cam->checkCulling(*offset, (*iter))) // The triangle is facing away from the camera
			continue;

		// TODO check for triangles outside of view before rendering
		//if (!cam->checkVisible())
			//continue;

		// Render the triangle by converting its projection on the camera's viewing plane into pixel coordinates
		pixelTriplet pixels(&(*iter));
		if (!cam->render(*offset, pixels)) // All vertices are behind camera
			continue;

		// Convert to pixel coordinates
		// (0, 0) is at the top-left of the screen
		if (!convertToPixelSpace(pixels)) // Check if the triangle is on the screen
			continue;

		// Rasterize the triangle
		if (!pixels.sortVertical(maxPixelsY))
			continue;

		// Set pointer to the offset vector of the parent object
		pixels.offset = offset;

		// Add the triangle to the drawing list
		polygonsToDraw.push_back(pixels);
	}
}

void scene::processObject(object *obj){
	// Compute all parent polygons
	processPolygons(obj);
	for (auto child = obj->beginChildren(); child != obj->endChildren(); child++) { // And for all the object's children
		processPolygons((*child));
	}
}

void scene::convertToScreenSpace(const int& px, const int& py, float& x, float& y) const {
	x = (2.f * px) / screenWidthPixels - 1;
	y = (-2.f * py) / screenHeightPixels + 1;
}

bool scene::checkScreenSpace(const float& x, const float& y) const {
	return ((x >= -SCREEN_XLIMIT && x <= SCREEN_XLIMIT) && (y >= -SCREEN_YLIMIT && y <= SCREEN_YLIMIT));
}

bool scene::convertToPixelSpace(const float& x, const float& y, int &px, int &py) const {
	px = (int)(screenWidthPixels*((x + 1)/2));
	py = (int)(screenHeightPixels*(1 - (y + 1)/2));
	return checkScreenSpace(x, y);
}

bool scene::convertToPixelSpace(pixelTriplet &coords) const {
	bool retval = false;
	for (size_t i = 0; i < 3; i++) {
		coords.set(i, (int)(screenWidthPixels * ((coords.sX[i] + 1) / 2)), 
		              (int)(screenHeightPixels * (1 - (coords.sY[i] + 1) / 2)));
		retval |= checkScreenSpace(coords.sX[i], coords.sY[i]);
	}
	return retval;
}

void scene::drawPoint(const vector3 &point, const ColorRGB &color){
	float cmX, cmY;
	if(cam->projectPoint(point, cmX, cmY)){
		int cmpX, cmpY;
	
		// Convert the screen-space coordinates to pixel-space
		if(!convertToPixelSpace(cmX, cmY, cmpX, cmpY)) // Check if the point is on the screen
			return;
		
		// Draw the normal vector
		window->setDrawColor(color);
		window->drawPixel(cmpX, cmpY);
	}
}

void scene::drawVector(const vector3 &start, const vector3 &direction, const ColorRGB &color, const float &length/*=1*/){
	// Compute the normal vector from the center of the triangle
	vector3 P = start + (direction * length);

	// Draw the triangle normals
	float cmX0, cmY0;
	float cmX1, cmY1;
	if(cam->projectPoint(start, cmX0, cmY0) && cam->projectPoint(P, cmX1, cmY1)){
		int cmpX0, cmpY0;
		int cmpX1, cmpY1;
		
		bool os1 = checkScreenSpace(cmX0, cmY0);
		bool os2 = checkScreenSpace(cmX1, cmY1);
		
		// Check if the line is on the screen
		if(!os1 && !os2) // The line is completely off the screen
			return;
		/*else if(!os1){ // (cmX0, cmY0) is off screen
			vector3 p(cmX0-cmX1, cmY0-cmY1);
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
			vector3 p(cmX1-cmX0, cmY1-cmY0);
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
}

void scene::drawRay(const ray &proj, const ColorRGB &color, const float &length/*=1*/){
	drawVector(proj.pos, proj.dir, color, length);
}

void scene::drawTriangle(const pixelTriplet &coords, const ColorRGB &color){
	window->setDrawColor(color);
	for(size_t i = 0; i < 2; i++)
		window->drawLine(coords.pX[i], coords.pY[i], coords.pX[i+1], coords.pY[i+1]);
	window->drawLine(coords.pX[2], coords.pY[2], coords.pX[0], coords.pY[0]);
}

void scene::drawFilledTriangle(const pixelTriplet &coords, const ColorRGB &color){
	// Set the fill color
	window->setDrawColor(color);

	int x0, x1;
	for(int scanline = coords.pY[0]; scanline <= coords.pY[2]; scanline++){
		if (!coords.getHorizontalLimits(scanline, x0, x1))
			continue;

		// Draw the scanline
		window->drawLine(x0, scanline, x1, scanline);
	}
}
