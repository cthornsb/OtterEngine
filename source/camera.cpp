#include <iostream>
#include <string>
#include <cmath>

#include "camera.hpp"
#include "object.hpp"

const double pi = 3.14159265359;
const double pi2 = 2 * pi;
const double deg2rad = pi/180;
const double rad2deg = 180/pi;

const vector3 upVector(0, 1, 0);

camera::camera() :
	fov(pi/2), // radians
	L(50E-3), // m
	A(640.0/480), 
	W(0),
	H(0),
	vPlane(),
	pos(),
	uX(1,0,0),
	uY(0,1,0),
	uZ(0,0,1),
	pitchAngle(0),
	rollAngle(0),
	yawAngle(0)
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

void camera::setFOV(const double &fov_){ 
	fov = fov_*deg2rad; 
	computeViewingPlane();
}

void camera::setFocalLength(const double &length){
	L = length*1E-3;
	computeViewingPlane();
}

void camera::setAspectRatio(const double &ratio){
	A = ratio;
	computeViewingPlane();
}

/////////////////////////////////////////////////
// Movement methods
/////////////////////////////////////////////////

void camera::moveForward(const double &dist){
	pos += uZ*dist;
	updateViewingPlane();
}

void camera::moveRight(const double &dist){
	pos += uX*dist;
	updateViewingPlane();
}

void camera::moveUp(const double &dist){
	pos += uY*dist;
	updateViewingPlane();	
}

void camera::move(const vector3 &displacement){
	pos += displacement;
	updateViewingPlane();
}

void camera::move(const double &x, const double &y, const double &z){
	pos += vector3(x, y, z);
	updateViewingPlane();
}

void camera::moveTo(const vector3 &position){ 
	pos = position; 
	updateViewingPlane();
}

void camera::moveTo(const double &x, const double &y, const double &z){
	pos = vector3(x, y, z);
	updateViewingPlane();
}

/////////////////////////////////////////////////
// Rotation methods
/////////////////////////////////////////////////

void camera::rotate(const double &pitch, const double &yaw, const double& roll/*=0*/){
	// Reset the camera to its original orientation
	resetUnitVectors();
	pitchAngle += pitch;
	if (pitchAngle > pi)
		pitchAngle = pi;
	else if (pitchAngle < -pi)
		pitchAngle = -pi;
	yawAngle += yaw;
	rollAngle += roll;
	setRotation(pitchAngle, rollAngle, yawAngle);
}

void camera::setRotation(const double &theta, const double &phi, const double &psi){
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

void camera::render(const vector3 &offset, const triangle &tri, double *sX, double *sY, bool *valid){
	valid[0] = projectPoint(*tri.p0+offset, sX[0], sY[0]);
	valid[1] = projectPoint(*tri.p1+offset, sX[1], sY[1]);
	valid[2] = projectPoint(*tri.p2+offset, sX[2], sY[2]);
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
