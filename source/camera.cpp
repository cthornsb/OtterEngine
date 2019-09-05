#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>

#include "camera.hpp"
#include "object.hpp"
#include "sdlWindow.hpp"

const double pi = 3.14159265359;
const double deg2rad = pi/180;
const double rad2deg = 180/pi;

camera::camera() : ray() { 
	dir = uZ; // Look down the local Z-axis
	initialize();
}

camera::camera(const vector3 &pos_) : ray() { 
	pos = pos_;
	dir = uZ; // Look down the local Z-axis
	initialize();
}

camera::camera(const vector3 &pos_, const vector3 &dir_) : ray(pos_, dir_) { 
	// Fix this! uZ is not correct since we set dir manually
	initialize();
}

camera::~camera(){
	// The SDL window's destructor will automatically handle its own clean-up
	delete window;
}

void camera::moveCam(const double &dist){
	pos = pos + uZ*dist;
	vPlane = plane(pos + uZ*L, uZ);
}

void camera::draw(object* obj, const drawMode &mode/*=WIREFRAME*/){
	std::vector<triangle>* polys = obj->getPolygons();
	vector3 offset = obj->getPosition();
	for(std::vector<triangle>::iterator iter = polys->begin(); iter != polys->end(); iter++){
		draw(offset, (*iter), mode);
	}
}

void camera::draw(const vector3 &offset, const triangle &tri, const drawMode &mode/*=WIREFRAME*/){
	// Do backface culling
	double dp = ((tri.p+offset) - pos) * tri.norm;
	if(mode != WIREFRAME && (dp > 0)) // The triangle is facing away from the camera
		return;

	double sX[3] = {0, 0, 0};
	double sY[3] = {0, 0, 0};
	
	compute(*tri.p0+offset, sX[0], sY[0]);
	compute(*tri.p1+offset, sX[1], sY[1]);
	compute(*tri.p2+offset, sX[2], sY[2]);
	
	// Convert to pixel coordinates
	// (0, 0) is at the top-left of the screen
	int pixelX[3], pixelY[3];
	for(size_t i = 0; i < 3; i++)
		convertToPixelSpace(sX[i], sY[i], pixelX[i], pixelY[i]);

	// Draw the triangle to the screen
	if(mode == WIREFRAME || mode == MESH){
		drawTriangle(pixelX, pixelY, Colors::WHITE);
	}
	else if(mode == SOLID){
		// Draw the triangle face and the outline of the triangle
		drawFilledTriangle(pixelX, pixelY, Colors::WHITE);
		
		// Draw the edges of the triangles
		drawTriangle(pixelX, pixelY, Colors::BLACK);
	}
	else if(mode == RENDER){
		// Compute the dot-product between the triangle normal and the light direction
		// When the normal is close to anti-parallel with the light direction, the triangle 
		//  will receive close to the maximum light level. When the angle between the two
		//  vectors approaches 90 degrees (and above), the triangle will receive almost no light.
		dp = -(light * tri.norm);
		if(dp < 0) dp = 0;
		sdlColor col(dp);
		drawFilledTriangle(pixelX, pixelY, col);
	}
	
	if(drawNorm) // Draw the surface normal vector
		drawVector(tri.p+offset, tri.norm, Colors::RED);
}

void camera::clear(const sdlColor &color/*=Colors::BLACK*/){
	window->clear();
}

void camera::update(){
	if(!window->status()){ // Check if the window has been closed
		isRunning = false;
		return;
	}
	window->render();
}

void camera::loop(){
	while(true){
		if(!window->status()) // Check if the window has been closed
			break;
		usleep(16700); // ~60 Hz
	}
}

void camera::dump() const {
	std::cout << " FOV:    " << fov*180/pi << " degrees\n";
	std::cout << " f:      " << L << " m\n";
	std::cout << " A:      " << A << "\n";
	std::cout << " Width:  " << W << " m\n";
	std::cout << " Height: " << H << " m\n";
	std::cout << " PixelX: " << pX << " m\n";
	std::cout << " PixelY: " << pY << " m\n";
	std::cout << " Light = (" << light.x << ", " << light.y << ", " << light.z << ")\n";
	std::cout << " unitX = (" << uX.x << ", " << uX.y << ", " << uX.z << ")\n";
	std::cout << " unitY = (" << uY.x << ", " << uY.y << ", " << uY.z << ")\n";
	std::cout << " unitZ = (" << uZ.x << ", " << uZ.y << ", " << uZ.z << ")\n";
}

void camera::initialize(){
	// Set the default screen dimensions
	screenWidthPixels = 640;
	screenHeightPixels = 480;

	// Set initial parameters
	fov = pi/2; // radians
	L = 50E-3; // m
	A = 16.0/9; // unitless
	drawNorm = false;
	isRunning = true;
	uX = vector3(1, 0, 0);
	uY = vector3(0, 1, 0);
	uZ = vector3(0, 0, 1);
	light = vector3(0, 0, 1);

	// Setup the viewing plane (looking down the Z-axis)
	vPlane = plane(pos + uZ*L, uZ);

	// Compute additional parameters
	W = 2*L*std::tan(fov/2); // m
	H = W/A; // m
	pX = W/screenWidthPixels; // m/pixel
	pY = H/screenHeightPixels; // m/pixel
	
	// Setup the window
	window = new sdlWindow(screenWidthPixels, screenHeightPixels);
	window->initialize();
}

void camera::convertToScreenSpace(const vector3 &vec, double &x, double &y){
	x = 2*(vec * uX)/W; // unitless
	y = 2*(vec * uY)/H; // unitless	
}

void camera::convertToPixelSpace(const double &x, const double &y, int &px, int &py){
	px = (int)((W/pX)*((x + 1)/2));
	py = (int)((H/pY)*(1 - (y + 1)/2));
}

bool camera::compute(const vector3 &vertex, double &sX, double &sY){
	ray proj(vertex, pos-vertex);
	
	double t;
	if(vPlane.intersects(proj, t)){
		// Get the point at which the ray intersect the viewing plane
		vector3 pp = proj.extend(t);

		// Convert the viewing plane point to screen space (-1, 1)
		convertToScreenSpace(pp, sX, sY);
		
		return true;
	}
	
	return false;
}

bool camera::rayTrace(const double &sX, const double &sY, const triangle &tri, vector3 &P){
	double x = (sX * W)/2;
	double y = (sY * H)/2;
	vector3 vec = (vPlane.p + uX*x + uY*y - pos).normalize();
	ray cast(pos, vec);
	double t;
	if(tri.intersects(cast, t)){
		P = cast.extend(t);
		return true;
	}
	return false;
}

void camera::drawVector(const vector3 &start, const vector3 &direction, const sdlColor &color, const double &length/*=1*/){
	// Compute the normal vector from the center of the triangle
	vector3 P = start + direction;

	// Draw the triangle normals
	double cmX0, cmY0;
	double cmX1, cmY1;
	if(compute(start, cmX0, cmY0) && compute(P, cmX1, cmY1)){
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

void camera::drawRay(const ray &proj, const sdlColor &color, const double &length/*=1*/){
	drawVector(proj.pos, proj.dir, color, length);
}

void camera::drawTriangle(const int *x, const int *y, const sdlColor &color){
	window->setDrawColor(color);
	for(size_t i = 0; i < 2; i++)
		window->drawLine(x[i], y[i], x[i+1], y[i+1]);
	window->drawLine(x[2], y[2], x[0], y[0]);
}
	
void camera::drawFilledTriangle(const int *x, const int *y, const sdlColor &color){
	int x0, y0;
	int x1, y1;
	int x2, y2;
	
	x0 = x[0]; y0 = y[0];
	x1 = x[1]; y1 = y[1];
	x2 = x[2]; y2 = y[2];

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
