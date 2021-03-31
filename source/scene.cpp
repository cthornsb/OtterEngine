#include <iostream>

#ifndef WIN32
#include <unistd.h>
#endif // ifndef WIN32

#include "scene.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "OTTWindow3D.hpp"
#include "OTTShader.hpp"

#ifdef SOFTWARE_RENDERER
#define SCREEN_XLIMIT 1.0 ///< Set the horizontal clipping border as a fraction of the total screen width
#define SCREEN_YLIMIT 1.0 ///< Set the vertical clipping border as a fraction of the total screen height
#endif // ifdef SOFTWARE_RENDERER

scene::scene() : 
	OTTFrameTimer(),
	drawNorm(false), 
	drawOrigin(false), 
	drawDepthMap(false),
	isRunning(true), 
	cam(0x0),
#ifndef SOFTWARE_RENDERER
	window(new OTTWindow3D(640, 480, 1)),
#else
	window(new OTTWindow(640, 480, 1)),
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
	initialize();
}

scene::scene(camera *cam_) : 
	scene()
{ 
	setCamera(cam_);
}

scene::~scene(){
}

void scene::initialize(){
	// Initialize the window
	window->initialize("Render3d");

#ifndef SOFTWARE_RENDERER
	// Switch to OpenGL renderer mode
	window->enableZDepth();
	window->enableCulling();
	window->enableAlphaBlending();
#else
	// Set the pixel coordinate bounds
	minPixelsX = (int)(screenWidthPixels*(1-SCREEN_YLIMIT)/2);
	maxPixelsX = (int)(screenWidthPixels-minPixelsX);
	minPixelsY = (int)(screenHeightPixels*(1-SCREEN_YLIMIT)/2);
	maxPixelsY = (int)(screenHeightPixels-minPixelsX);
#endif // ifdef SOFTWARE_RENDERER

	// Add the world light to the list of light sources
	addLight(&worldLight);
}

void scene::enableVSync() {
	if (!window)
		return;
	window->enableVSync();
	disableFramerateCap(); // Disable built-in frame timer
}

void scene::disableVSync(const double& fps/*=60.0*/) {
	if (!window)
		return;
	window->disableVSync();
	setFramerateCap(fps); // Update the frame timer with the new target framerate
}

void scene::addObject(object* obj) { 
	obj->build(); // Construct the object
	if(!obj->getShader()) // Set default shader
		obj->setShader(window->getShader(ShaderType::DEFAULT));
	objects.push_back(obj); // Add it to the list of objects to draw
}

void scene::clear(const ColorRGB &color/*=Colors::BLACK*/){
	window->clear(color);
}

#ifndef SOFTWARE_RENDERER
bool scene::update() {
	// Clear the screen with a color
	clear(Colors::BLACK);

	// Update projection and camera view transformation matrices
	Matrix4* proj = cam->getProjectionMatrix();
	Matrix4* view = cam->getViewMatrix();

	// Draw the origin
	if (drawOrigin) { 
		Matrix4 mvp = Matrix4::concatenate(proj, view, &identityMatrix4);
		window->enableShader(ShaderType::DEFAULT);
		window->getShader(ShaderType::DEFAULT)->setMatrix4("MVP", &mvp);
		drawAxes();
		window->disableShader();
	}

	// Draw the vertices of all objects (using OpenGL)
	//window->enableWireframeMode();
	window->setDrawColor(Colors::WHITE); // Set draw color (for default shader)
	for (auto obj = objects.cbegin(); obj != objects.cend(); obj++) {
		// Setup MVP matrices
		Matrix4* model = (*obj)->getModelMatrix();
		Matrix4 mvp = Matrix4::concatenate(proj, view, model);

		// Setup object shader
		const OTTShader* shdr = (*obj)->getShader();
		shdr->enableShader();
		shdr->setMatrix4("MVP", &mvp);
		Vector3 camPos = cam->getPosition();
		Vector3 camDir = cam->getDirection();
		shdr->setVector3("camPos", &camPos);
		shdr->setVector3("camDir", &camDir);

		// Draw all sub-objects
		(*obj)->draw(window.get());

		// Disable object shader
		shdr->disableShader();

		// Debugging
		if (drawOrigin || drawNorm) {
			window->enableShader(ShaderType::SIMPLE);
			window->getShader(ShaderType::DEFAULT)->setMatrix4("MVP", &mvp);
			if (drawOrigin) { // Draw object unit vectors
				drawAxes();
			}
			if (drawNorm) { // Draw face normals
				std::vector<triangle>* polys = (*obj)->getPolygonVector();
				window->setDrawColor(Colors::CYAN);
				for (auto tri = polys->cbegin(); tri != polys->cend(); tri++) {
					Vector3 base = tri->getInitialCenterPoint();
					Vector3 tip = base + tri->getInitialNormal() * 0.25f;
					window->drawLine(base, tip);
				}
			}
			window->disableShader();
		}
	}

	return render();
}
#else
bool scene::update(){
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
		int x0, x1;
		float realy;
		const float farDistance = 10.f;
		for (std::vector<pixelTriplet>::iterator triplet = polygonsToDraw.begin(); triplet != polygonsToDraw.end(); triplet++) {
			for (int scanline = 0; scanline < maxPixelsY; scanline++) {
				realy = (-2.f * scanline) / screenHeightPixels + 1;
				if (triplet->getHorizontalLimits(scanline, x0, x1)) { // Rasterization of triangle
					// TODO fix object vertices being rendered multiple times
					for (int x = x0; x <= x1; x++) { // Over every pixel in the row
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
		for (unsigned short y = 0; y < maxPixelsY; y++) {
			realy = (-2.f * y) / screenHeightPixels + 1;
			for (unsigned short x = 0; x < maxPixelsX; x++) {
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
		int x0, x1;
		const float farDistance = 10.f;
		for (std::vector<pixelTriplet>::const_iterator triplet = polygonsToDraw.cbegin(); triplet != polygonsToDraw.cend(); triplet++) { // Over all polygons to draw
			for (int scanline = triplet->p0->pY; scanline <= triplet->p2->pY; scanline++) {
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

bool scene::render() {
	// Update the screen	
	if (!window->processEvents()) { // Check for events
		isRunning = false;
		return false;
	}
	window->setCurrent();
	window->render();
	return true;
}

OTTKeyboard* scene::getKeypress(){
	return window->getKeypress();
}

OTTMouse* scene::getMouse(){
	return window->getMouse();
}

void scene::setCamera(camera *cam_){ 
	cam = cam_; 
	cam->setAspectRatio(window->getAspectRatio());	
	cam->setPerspectiveProjection(0.1f, 100.f); // Setup the camera projection matrix
}

#ifdef SOFTWARE_RENDERER
void scene::processPolygons(object* obj) {
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
		coords.set(i, (int)(screenWidthPixels * ((coords[i]->sX + 1) / 2)),
		              (int)(screenHeightPixels * (1 - (coords[i]->sY + 1) / 2)));
		retval |= checkScreenSpace(coords[i]->sX, coords[i]->sY);
	}
	return retval;
}
#endif // ifndef SOFTWARE_RENDERER

void scene::drawPoint(const Vector3 &point, const ColorRGB &color){
#ifndef SOFTWARE_RENDERER
	window->drawPixel(point[0], point[1], point[2]);
#else
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
#endif // ifndef SOFTWARE_RENDERER
}

void scene::drawVector(const Vector3 &start, const Vector3 &direction, const ColorRGB &color, const float &length/*=1*/){
#ifndef SOFTWARE_RENDERER
	window->drawLine(start, start + (direction * length));
#else
	// Compute the end point
	Vector3 P = start + (direction * length);

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

void scene::drawRay(const ray &proj, const ColorRGB &color, const float &length/*=1*/){
	drawVector(proj.pos, proj.dir, color, length);
}

#ifdef SOFTWARE_RENDERER
void scene::drawTriangle(const pixelTriplet &coords, const ColorRGB &color){
	window->setDrawColor(color);
	for(size_t i = 0; i < 2; i++)
		window->drawLine(coords[i]->pX, coords[i]->pY, coords[i+1]->pX, coords[i+1]->pY);
	window->drawLine(coords[2]->pX, coords[2]->pY, coords[0]->pX, coords[0]->pY);
}

void scene::drawFilledTriangle(const pixelTriplet &coords, const ColorRGB &color){
	// Set the fill color
	window->setDrawColor(color);

	int x0, x1;
	for(int scanline = coords[0]->pY; scanline <= coords[2]->pY; scanline++){
		if (!coords.getHorizontalLimits(scanline, x0, x1))
			continue;

		// Draw the scanline
		window->drawLine(x0, scanline, x1, scanline);
	}
}
#endif // ifndef SOFTWARE_RENDERER

void scene::drawAxes() {
	window->setDrawColor(Colors::RED);
	window->drawLine(0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
	window->setDrawColor(Colors::GREEN);
	window->drawLine(0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
	window->setDrawColor(Colors::BLUE);
	window->drawLine(0.f, 0.f, 0.f, 0.f, 0.f, 1.f);
}
