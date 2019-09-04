#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>

#include "camera.hpp"
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
	delete window;
}

void camera::moveCam(const double &dist){
	pos = pos + uZ*dist;
	vPlane = plane(pos + uZ*L, uZ);
}

void camera::draw(std::vector<triangle> *tri, const drawMode &mode/*=WIREFRAME*/){
	for(std::vector<triangle>::iterator iter = tri->begin(); iter != tri->end(); iter++){
		draw((*iter), mode);
	}
}

void camera::draw(const triangle &tri, const drawMode &mode/*=WIREFRAME*/){
	double sX[3] = {0, 0, 0};
	double sY[3] = {0, 0, 0};
	
	compute(tri.p0, sX[0], sY[0]);
	compute(tri.p1, sX[1], sY[1]);
	compute(tri.p2, sX[2], sY[2]);
	
	// Convert to pixel coordinates
	// (0, 0) is at the top-left of the screen
	int pixelX[3], pixelY[3];
	for(size_t i = 0; i < 3; i++)
		convertToPixelSpace(sX[i], sY[i], pixelX[i], pixelY[i]);

	// Do backface culling
	double dp = (tri.p - pos) * tri.norm;
	if(mode != WIREFRAME && (dp > 0))
		return;

	// Draw the triangle to the screen
	if(mode == WIREFRAME || mode == MESH){
		window->setDrawColor(Colors::WHITE);
		window->drawTriangle(pixelX, pixelY);
	}
	else if(mode == SOLID){
		// Draw the triangle face
		window->setDrawColor(Colors::WHITE);
		window->drawTriangle(pixelX, pixelY, true);
		
		// Draw the outline of the triangle
		window->setDrawColor(Colors::BLACK);
		window->drawTriangle(pixelX, pixelY);
	}
	else if(mode == RENDER){
		// Compute the dot-product between the triangle normal and the light direction
		// When the normal is close to anti-parallel with the light direction, the triangle 
		//  will receive close to the maximum light level. When the angle between the two
		//  vectors approaches 90 degrees (and above), the triangle will receive almost no light.
		dp = -(light * tri.norm);
		if(dp < 0) dp = 0;
		sdlColor col(dp);
		window->setDrawColor(col);
		window->drawTriangle(pixelX, pixelY, true);
	}
	
	if(drawNorm){
		// Compute the normal vector from the center of the triangle
		vector3 P = tri.p + tri.norm;

		// Draw the triangle normals
		double cmX0, cmY0;
		double cmX1, cmY1;
		if(compute(tri.p, cmX0, cmY0) && compute(P, cmX1, cmY1)){
			int cmpX0, cmpY0;
			int cmpX1, cmpY1;
			
			// Convert the screen-space coordinates to pixel-space
			convertToPixelSpace(cmX0, cmY0, cmpX0, cmpY0);
			convertToPixelSpace(cmX1, cmY1, cmpX1, cmpY1);
			
			// Draw the normal vector
			window->setDrawColor(Colors::WHITE);
			window->drawLine(cmpX0, cmpY0, cmpX1, cmpY1);
		}
	}
}

void camera::clear(){
	window->clear();
}

void camera::render(){
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
	std::cout << " uX = (" << uX.x << ", " << uX.y << ", " << uX.z << ")\n";
	std::cout << " uY = (" << uY.x << ", " << uY.y << ", " << uY.z << ")\n";
	std::cout << " uZ = (" << uZ.x << ", " << uZ.y << ", " << uZ.z << ")\n";
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
	uX = vector3(1, 0, 0);
	uY = vector3(0, 1, 0);
	uZ = vector3(0, 0, 1);
	light = vector3(0, -1, 0);

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
	//px = (int)(screenWidthPixels * 0.5 * (x/pX + 1));
	//py = (int)(screenHeightPixels * (1 - 0.5*(y/pY + 1)));
}

bool camera::compute(const vector3 &vertex, double &sX, double &sY){
	ray proj(vertex, pos-vertex);
	
	/*if(mode == CULLED){
		if(proj.dir * tri.norm < 0)
			return;
	}*/
	
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
