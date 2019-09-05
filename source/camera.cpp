#include <iostream>
#include <string>
#include <cmath>

#include "camera.hpp"
#include "object.hpp"

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
}

void camera::moveCam(const double &dist){
	pos = pos + uZ*dist;
	vPlane = plane(pos + uZ*L, uZ);
}

void camera::render(const vector3 &offset, const triangle &tri, double *sX, double *sY){
	projectPoint(*tri.p0+offset, sX[0], sY[0]);
	projectPoint(*tri.p1+offset, sX[1], sY[1]);
	projectPoint(*tri.p2+offset, sX[2], sY[2]);
}

bool camera::checkCulling(const vector3 &offset, const triangle &tri){
	// Check if the triangle is facing towards the camera
	return (((tri.p+offset) - pos) * tri.norm <= 0);
}

bool camera::projectPoint(const vector3 &vertex, double &sX, double &sY){
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

void camera::dump() const {
	std::cout << " FOV:    " << fov*180/pi << " degrees\n";
	std::cout << " f:      " << L << " m\n";
	std::cout << " A:      " << A << "\n";
	std::cout << " Width:  " << W << " m\n";
	std::cout << " Height: " << H << " m\n";
	std::cout << " unitX = (" << uX.x << ", " << uX.y << ", " << uX.z << ")\n";
	std::cout << " unitY = (" << uY.x << ", " << uY.y << ", " << uY.z << ")\n";
	std::cout << " unitZ = (" << uZ.x << ", " << uZ.y << ", " << uZ.z << ")\n";
}

void camera::initialize(){
	// Set initial parameters
	fov = pi/2; // radians
	L = 50E-3; // m
	A = 16.0/9; // unitless
	uX = vector3(1, 0, 0);
	uY = vector3(0, 1, 0);
	uZ = vector3(0, 0, 1);

	// Setup the viewing plane (looking down the Z-axis)
	vPlane = plane(pos + uZ*L, uZ);

	// Compute additional parameters
	W = 2*L*std::tan(fov/2); // m
	H = W/A; // m
}

void camera::convertToScreenSpace(const vector3 &vec, double &x, double &y){
	x = 2*(vec * uX)/W; // unitless
	y = 2*(vec * uY)/H; // unitless	
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
