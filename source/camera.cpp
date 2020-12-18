#include <iostream>
#include <string>
#include <cmath>

#include "camera.hpp"
#include "object.hpp"
#include "Globals.hpp"

const vector3 upVector(0, 1, 0);

float WrappedValue::wrap(const float& d) const {
	float retval = value + d;
	if (!lock) {
		retval = fmod((value + d) - minVal, delta);
		if (retval < 0) {
			if (!lock)
				retval += delta;
			else
				retval = 0;
		}
		return retval + minVal;
	}
	else if (retval < minVal)
		return minVal;
	else if (retval > maxVal)
		return maxVal;
	return retval;
}

camera::camera() :
	fov(pi/2), // radians
	L(50E-3f), // m
	A(640.0f/480), 
	W(0),
	H(0),
	vPlane(),
	pos(),
	uX(1,0,0),
	uY(0,1,0),
	uZ(0,0,1),
	pitchAngle(0, -pi/2, pi/2, true),
	rollAngle(0, -pi/2, pi/2, true),
	yawAngle(0, -pi, pi)
{ 
	initialize();
}

camera::camera(const vector3 &pos_) :
	camera()
{ 
	pos = pos_;
	initialize();
}

camera::camera(const vector3 &pos_, const vector3 &dir_) :
	camera() 
{
	// Fix this! uZ is not correct since we set dir manually
	initialize();
}

camera::~camera(){
}

void camera::setFOV(const float &fov_){ 
	fov = fov_*deg2rad; 
	computeViewingPlane();
}

void camera::setFocalLength(const float &length){
	L = length*1E-3f;
	computeViewingPlane();
}

void camera::setAspectRatio(const float &ratio){
	A = ratio;
	computeViewingPlane();
}

/////////////////////////////////////////////////
// Movement methods
/////////////////////////////////////////////////

void camera::moveForward(const float &dist){
	pos += uZ*dist;
	updateViewingPlane();
}

void camera::moveRight(const float&dist){
	pos += uX*dist;
	updateViewingPlane();
}

void camera::moveUp(const float&dist){
	pos += uY*dist;
	updateViewingPlane();	
}

void camera::move(const vector3 &displacement){
	pos += displacement;
	updateViewingPlane();
}

void camera::move(const float&x, const float&y, const float&z){
	pos += vector3(x, y, z);
	updateViewingPlane();
}

void camera::moveTo(const vector3 &position){ 
	pos = position; 
	updateViewingPlane();
}

void camera::moveTo(const float&x, const float&y, const float&z){
	pos = vector3(x, y, z);
	updateViewingPlane();
}

/////////////////////////////////////////////////
// Rotation methods
/////////////////////////////////////////////////

void camera::rotate(const float& pitch, const float& yaw, const float& roll/*=0*/){
	// Reset the camera to its original orientation
	resetUnitVectors();
	pitchAngle += pitch;
	yawAngle += yaw;
	rollAngle += roll;
	setRotation(pitchAngle.get(), rollAngle.get(), yawAngle.get());
}

void camera::setRotation(const float& theta, const float& phi, const float& psi){
	// Reset the camera to its original orientation
	resetUnitVectors();
	
	// Get the rotation matrix
	matrix3 rot(theta, phi, psi);

	// Update the rotation of the three unit vectors
	rot.transform(uX);
	rot.transform(uY);
	rot.transform(uZ);

	// Update the viewing plane
	updateViewingPlane();
}

void camera::lookAt(const vector3 &position){
	// Point the Z axis unit vector at the point in space
	uZ = (position - pos).normalize();

	// Compute the Y-axis unit vector  	
	matrix3 pitch = matrix3::getPitchMatrix(std::acos(uZ.y) - pi/2);
	uY = upVector;
	pitch.transform(uY);
	
	// Get the X-axis unit vector by computing the cross product of the Y and Z vectors
	uX = uY.cross(uZ);
	
	// Update the viewing plane
	updateViewingPlane();
}

void camera::resetOrientation(){
	resetUnitVectors();
	updateViewingPlane();
	pitchAngle = 0;
	rollAngle = 0;
	yawAngle = 0;
}

/////////////////////////////////////////////////
// Rendering methods
/////////////////////////////////////////////////

void camera::render(const vector3 &offset, const triangle &tri, float* sX, float* sY, bool* valid){
	valid[0] = projectPoint(*tri.p0+offset, sX[0], sY[0]);
	valid[1] = projectPoint(*tri.p1+offset, sX[1], sY[1]);
	valid[2] = projectPoint(*tri.p2+offset, sX[2], sY[2]);
}

bool camera::checkCulling(const vector3 &offset, const triangle &tri){
	// Check if the triangle is facing towards the camera
	return (((tri.p+offset) - pos) * tri.norm <= 0);
}

bool camera::projectPoint(const vector3 &vertex, float& sX, float& sY){
	ray proj(vertex, pos-vertex);
	
	float t;
	if(vPlane.intersects(proj, t)){
		// Get the point at which the ray intersect the viewing plane
		vector3 pp = proj.extend(t) - vPlane.p;

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
	resetUnitVectors();

	// Setup the viewing plane (looking down the Z-axis)
	updateViewingPlane();

	// Compute the dimensions of the viewing plane
	computeViewingPlane();
}

void camera::computeViewingPlane(){
	W = 2*L*std::tan(fov/2); // m
	H = W/A; // m
}

void camera::updateViewingPlane(){
	vPlane.p = pos + uZ*L;
	vPlane.norm = uZ;
}

void camera::resetUnitVectors() {
	uX = vector3(1, 0, 0);
	uY = vector3(0, 1, 0);
	uZ = vector3(0, 0, 1);
}

void camera::convertToScreenSpace(const vector3 &vec, float& x, float& y){
	x = 2*(vec * uX)/W; // unitless
	y = 2*(vec * uY)/H; // unitless
}

bool camera::rayTrace(const float& sX, const float& sY, const triangle &tri, vector3 &P){
	float x = (sX * W)/2;
	float y = (sY * H)/2;
	vector3 vec = (vPlane.p + uX*x + uY*y - pos).normalize();
	ray cast(pos, vec);
	float t;
	if(tri.intersects(cast, t)){
		P = cast.extend(t);
		return true;
	}
	return false;
}
