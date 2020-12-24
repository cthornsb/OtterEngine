#include <iostream>
#include <string>
//#include <cmath>

#include "camera.hpp"
#include "object.hpp"
#include "Globals.hpp"

const vector3 upVector(0, 1, 0);

camera::camera() :
	plane(),
	fov(pi/2), // radians
	L(50E-3f), // m
	A(640.0f/480), 
	W(0),
	H(0),
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
	setRotation((pitchAngle += pitch), (rollAngle += roll), (yawAngle += yaw));
}

void camera::setRotation(const float& theta, const float& phi, const float& psi){
	// Reset the camera to its original orientation
	resetUnitVectors();
	
	// Get the rotation matrix
	matrix3 rot(theta, phi, psi);

	// Update the rotation of the three unit vectors
	rot.transformInPlace(uX);
	rot.transformInPlace(uY);
	rot.transformInPlace(uZ);

	// Update the viewing plane
	updateViewingPlane();
}

void camera::lookAt(const vector3 &position){
	// Point the Z axis unit vector at the point in space
	uZ = (position - pos).normalize();

	// Compute the Y-axis unit vector  	
	matrix3 pitch = matrix3::getPitchMatrix(std::acos(uZ.y) - pi/2);
	uY = upVector;
	pitch.transformInPlace(uY);
	
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

bool camera::render(pixelTriplet& pixels){
	bool retval = true;
	retval &= projectPoint(pixels[0]->getPosition(), pixels[0]->sX, pixels[0]->sY, &pixels[0]->zDepth);
	retval &= projectPoint(pixels[1]->getPosition(), pixels[1]->sX, pixels[1]->sY, &pixels[1]->zDepth);
	retval &= projectPoint(pixels[2]->getPosition(), pixels[2]->sX, pixels[2]->sY, &pixels[2]->zDepth);
	return retval;
}

bool camera::render(Vertex& vertex) {
	return projectPoint(vertex.getPosition(), vertex.sX, vertex.sY, &vertex.zDepth);
}

bool camera::checkCulling(const vector3 &offset, const triangle &tri){
	// Check if the triangle is facing towards the camera
	return ((tri.getCenterPoint() - pos) * tri.getNormal() <= 0);
}

bool camera::projectPoint(const vector3 &vertex, float& sX, float& sY, float* zdepth/*=0x0*/){
	vector3 vec = pos - vertex;
	ray proj(vertex, vec);
	
	if (zdepth) {
		(*zdepth) = -(vec.length() * vec.cosTheta(uZ));
		if (*zdepth < 0) // Vertex behind camera
			return false;
	}

	float t;
	if(intersects(proj, t)){
		// Get the point at which the ray intersect the viewing plane
		vector3 pp = proj.extend(t) - p;

		// Convert the viewing plane point to screen space (-1, 1)
		convertToScreenSpace(pp, sX, sY);
		
		return true;
	}
	
	return false;
}

float camera::getZDepth(const vector3& p) const {
	vector3 zDepth = p - pos; // Vector from focal point of camera to center of polygon
	return (zDepth.length() * zDepth.cosTheta(uZ));
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
	p = pos + uZ*L;
	norm = uZ;
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

bool camera::rayTrace(const ray& cast, const pixelTriplet& pixels, vector3& P) const {
	float t;
	if(pixels.intersects(cast, t)){
		P = cast.extend(t);
		return true;
	}
	return false;
}
