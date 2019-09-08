#include <iostream>
#include <unistd.h>

#include "scene.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "sdlWindow.hpp"

scene::scene() : timeElapsed(0), totalRenderTime(0), renderTime(0), framerate(0), framerateCap(60), updateCount(0), 
                 drawNorm(false), drawOrigin(false), isRunning(true), 
                 screenWidthPixels(640), screenHeightPixels(480), cam(NULL) { 
	initialize();
}

scene::scene(camera *cam_) : timeElapsed(0), totalRenderTime(0), renderTime(0), framerate(0), framerateCap(60), updateCount(0),
                             drawNorm(false), drawOrigin(false), isRunning(true), 
                             screenWidthPixels(640), screenHeightPixels(480), cam(cam_) { 
	initialize();
	setCamera(cam_);
}

scene::~scene(){
	// The SDL window's destructor will automatically handle its own clean-up
	delete window;
}

void scene::initialize(){
	// Initialize the high resolution time
	timeOfInitialization = sclock::now();
	timeOfLastUpdate = sclock::now();

	// Setup the window
	window = new sdlWindow(screenWidthPixels, screenHeightPixels);
	window->initialize();
}

void scene::clear(const sdlColor &color/*=Colors::BLACK*/){
	window->clear(color);
}

bool scene::update(){
	// Update the timer
	timeOfLastUpdate = sclock::now();

	// Start the render timer
	sclock::time_point startOfRenderScene = sclock::now();
	
	// Clear the vector of triangles to draw
	polygonsToDraw.clear();

	// Clear the screen with a color
	clear(Colors::BLACK);
	
	// Draw the 3d geometry
	for(auto obj : objects)
		processObject(obj);
	
	// Draw rendered polygons
	if(!polygonsToDraw.empty()){
		for(auto triplet : polygonsToDraw){
			sdlColor col = worldLight.getColor(triplet.tri);
			drawFilledTriangle(triplet, col);
		}
	}

	if(drawOrigin){ // Draw the origin
		drawVector(vector3(0, 0, 0), vector3(1, 0, 0), Colors::RED);
		drawVector(vector3(0, 0, 0), vector3(0, 1, 0), Colors::GREEN);
		drawVector(vector3(0, 0, 0), vector3(0, 0, 1), Colors::BLUE);
	}

	// Update the screen
	if(!window->status()){ // Check for events
		isRunning = false;
		return false;
	}
	window->render();
	
	updateCount++;

	// Stop the render timer
	renderTime = std::chrono::duration_cast<std::chrono::duration<double>>(sclock::now() - startOfRenderScene).count();

	// Update the total render time and the instantaneous framerate
	totalRenderTime += renderTime;
	framerate = 1/renderTime;

	// Cap the framerate by sleeping	
	if(framerateCap > 0){
		int timeToSleep((1.0/framerateCap - renderTime)*1E6); // in microseconds
		if(timeToSleep > 0){
			usleep(timeToSleep);
			framerate = 1/(renderTime + timeToSleep*1E-6);
		}
	}

	// Get the time since the scene was initialized
	timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(sclock::now() - timeOfInitialization).count();
	
	return true;
}

void scene::wait(){
	/*while(true){
		if(!window->status()) // Check if the window has been closed
			break;
		usleep(16700); // ~60 Hz
	}*/
}

sdlKeypress* scene::getKeypress(){
	return window->getKeypress();
}

void scene::setCamera(camera *cam_){ 
	cam = cam_; 
	cam->setAspectRatio(double(screenWidthPixels)/screenHeightPixels);
}

void scene::processObject(object *obj){
	std::vector<triangle>* polys = obj->getPolygons();
	vector3 offset = obj->getPosition();
	drawMode mode = obj->getDrawingMode();
	for(std::vector<triangle>::iterator iter = polys->begin(); iter != polys->end(); iter++){
		// Do backface culling
		if(mode != WIREFRAME && !cam->checkCulling(offset, (*iter))) // The triangle is facing away from the camera
			continue;
		
		// Render the triangle by converting its projection on the camera's viewing plane into pixel coordinates
		double sX[3], sY[3];
		cam->render(offset, (*iter), sX, sY);
		
		// Convert to pixel coordinates
		// (0, 0) is at the top-left of the screen
		pixelTriplet pixels(&(*iter));
		convertToPixelSpace(sX, sY, pixels);
		
		// Draw the triangle to the screen
		if(mode == WIREFRAME || mode == MESH){
			drawTriangle(pixels, Colors::WHITE);
		}
		else if(mode == SOLID){
			// Draw the triangle face and the outline of the triangle
			drawFilledTriangle(pixels, Colors::WHITE);
		
			// Draw the edges of the triangles
			drawTriangle(pixels, Colors::BLACK);
		}
		else if(mode == RENDER){
			// Do nothing for now. Rendering is more complex than wireframe or solid mesh drawing
			//  because we need to take lighting into account. Add the projected triangle to the
			//  vector of good vertices for future drawing.
			polygonsToDraw.push_back(pixels);
		}
		
		if(drawNorm) // Draw the surface normal vector
			drawVector(iter->p+offset, iter->norm, Colors::RED);
	}
}

void scene::convertToPixelSpace(const double &x, const double &y, int &px, int &py){
	px = (int)(screenWidthPixels*((x + 1)/2));
	py = (int)(screenHeightPixels*(1 - (y + 1)/2));
}

void scene::convertToPixelSpace(const double *x, const double *y, pixelTriplet &coords){
	for(size_t i = 0; i < 3; i++){
		coords.pX[i] = (int)(screenWidthPixels*((x[i] + 1)/2));
		coords.pY[i] = (int)(screenHeightPixels*(1 - (y[i] + 1)/2));
	}
}

void scene::drawPoint(const vector3 &point, const sdlColor &color){
	double cmX, cmY;
	if(cam->projectPoint(point, cmX, cmY)){
		int cmpX, cmpY;
	
		// Convert the screen-space coordinates to pixel-space
		convertToPixelSpace(cmX, cmY, cmpX, cmpY);
		
		// Draw the normal vector
		window->setDrawColor(color);
		window->drawPixel(cmpX, cmpY);
	}
}

void scene::drawVector(const vector3 &start, const vector3 &direction, const sdlColor &color, const double &length/*=1*/){
	// Compute the normal vector from the center of the triangle
	vector3 P = start + direction;

	// Draw the triangle normals
	double cmX0, cmY0;
	double cmX1, cmY1;
	if(cam->projectPoint(start, cmX0, cmY0) && cam->projectPoint(P, cmX1, cmY1)){
		int cmpX0, cmpY0;
		int cmpX1, cmpY1;
		
		// Convert the screen-space coordinates to pixel-space
		convertToPixelSpace(cmX0, cmY0, cmpX0, cmpY0);
		convertToPixelSpace(cmX1, cmY1, cmpX1, cmpY1);
		
		// Draw the normal vector
		window->setDrawColor(color);
		window->drawLine(cmpX0, cmpY0, cmpX1, cmpY1);
	}
}

void scene::drawRay(const ray &proj, const sdlColor &color, const double &length/*=1*/){
	drawVector(proj.pos, proj.dir, color, length);
}

void scene::drawTriangle(const pixelTriplet &coords, const sdlColor &color){
	window->setDrawColor(color);
	for(size_t i = 0; i < 2; i++)
		window->drawLine(coords.pX[i], coords.pY[i], coords.pX[i+1], coords.pY[i+1]);
	window->drawLine(coords.pX[2], coords.pY[2], coords.pX[0], coords.pY[0]);
}
	
void scene::drawFilledTriangle(const pixelTriplet &coords, const sdlColor &color){
	int x0, y0;
	int x1, y1;
	int x2, y2;
	
	x0 = coords.pX[0]; y0 = coords.pY[0];
	x1 = coords.pX[1]; y1 = coords.pY[1];
	x2 = coords.pX[2]; y2 = coords.pY[2];

	// Sort points by ascending Y
	// Insertion sort
	if(y1 < y0){
		std::swap(y0, y1);
		std::swap(x0, x1);
	}
	if(y2 < y1){
		std::swap(y1, y2);
		std::swap(x1, x2);
		if(y1 < y0){
			std::swap(y1, y0);
			std::swap(x1, x0);
		}
	}
	
	// Set the fill color
	window->setDrawColor(color);
	
	float xA, xB;
	for(int scanline = y0; scanline <= y2; scanline++){
		if(y0 == y1){ // y10 is a horizontal line
				xA = (scanline-y1)*(float(x2-x1))/(y2-y1) + x1;
				xB = (scanline-y2)*(float(x0-x2))/(y0-y2) + x2;				
		}
		else if(y0 == y2){ // y20 is a horizontal line
				xA = (scanline-y1)*(float(x2-x1))/(y2-y1) + x1;
				xB = (scanline-y0)*(float(x1-x0))/(y1-y0) + x0;			
		}
		else{ // No lines are horizontal
			if(scanline <= y1)
				xA = (scanline-y0)*(float(x1-x0))/(y1-y0) + x0;
			else
				xA = (scanline-y1)*(float(x2-x1))/(y2-y1) + x1;
			xB = (scanline-y2)*(float(x0-x2))/(y0-y2) + x2;
		}

		if(xB < xA) // Sort xA and xB
			std::swap(xA, xB);

		// Draw the scanline
		window->drawLine((int)xA, scanline, (int)xB, scanline);
	}
}
