#include <iostream>
#include <string>

#include "camera.hpp"
#include "object.hpp"
#include "Globals.hpp"

camera::camera() :
	plane(),
	fov(pi/2), // radians
	L(50E-3f), // m
	A(640.0f/480), 
	W(0),
	H(0),
	position(),
	uX(1,0,0),
	uY(0,1,0),
	uZ(0,0,1),
	pitchAngle(0, -pi/2, pi/2, true),
	rollAngle(0, -pi/2, pi/2, true),
	yawAngle(0, -pi, pi)
{ 
	initialize();
}

camera::camera(const Vector3 &pos_) :
	camera()
{ 
	position = pos_;
	initialize();
}

camera::camera(const Vector3 &pos_, const Vector3 &dir_) :
	camera() 
{
	position = pos_;
	initialize();
	setDirection(dir_);
}

camera::~camera(){
}

Matrix4* camera::getViewMatrix() {
	// Update the view matrix as the camera may have moved
	viewMatrix.setViewMatrixFPS(position, rotation); // Construct view matrix from position and rotation matrix
	return &viewMatrix;
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
	position += uZ*dist;
	updateViewingPlane();
}

void camera::moveRight(const float&dist){
	position += uX*dist;
	updateViewingPlane();
}

void camera::moveUp(const float&dist){
	position += uY*dist;
	updateViewingPlane();	
}

void camera::move(const Vector3 &displacement){
	position += displacement;
	updateViewingPlane();
}

void camera::move(const float&x, const float&y, const float&z){
	position += Vector3(x, y, z);
	updateViewingPlane();
}

void camera::moveTo(const Vector3 &pos){ 
	position = position;
	updateViewingPlane();
}

void camera::moveTo(const float&x, const float&y, const float&z){
	position = Vector3(x, y, z);
	updateViewingPlane();
}

/////////////////////////////////////////////////////////////////////
// Rotation methods
/////////////////////////////////////////////////////////////////////

void camera::rotate(const float& pitch, const float& yaw, const float& roll){
	// Rotate the camera relative to its current orientation
	setRotation((pitchAngle += pitch), (yawAngle += yaw), (rollAngle += roll));
}

void camera::setRotation(const float& pitch, const float& yaw, const float& roll){
	// Get the rotation matrix
	rotation.setRotation(pitch, roll, yaw);

	// Update the rotation of the three unit vectors
	uX = rotation.transform(unitVectorX);
	uY = rotation.transform(unitVectorY);
	uZ = rotation.transform(unitVectorZ);
	uZ.invert(); // Necessary in OpenGL render mode

	// Update the viewing plane
	updateViewingPlane();
}

void camera::rotateFPS(const float& pitch, const float& yaw) {
	// Rotate the camera relative to its current orientation
	setRotationFPS((pitchAngle += pitch), (yawAngle += yaw));
}

void camera::setRotationFPS(const float& pitch, const float& yaw) {
	// Get the rotation matrix
	rotation = Matrix3::getFPSCameraMatrix((pitchAngle = pitch), (yawAngle = yaw));

	// Update the rotation of the three unit vectors
	uX = rotation.transform(unitVectorX);
	uY = rotation.transform(unitVectorY);
	uZ = rotation.transform(unitVectorZ);
	uZ.invert(); // Necessary in OpenGL render mode

	// Update the viewing plane
	updateViewingPlane();
}

void camera::setDirection(const Vector3& dir, const Vector3& upVector/* = unitVectorY*/) {
	lookAt(position + dir, upVector);
}

void camera::lookAt(const Vector3 &position, const Vector3& upVector/* = unitVectorY*/){
	// Point the Z axis unit vector at the point in space
	uZ = (position - position).normalize();
	uZ.invert(); // Necessary in OpenGL render mode

	// Compute the X-axis unit vector ("right" vector)
	uX = unitVectorY.cross(uZ).normalize();
		
	// Get the Y-axis unit vector by computing the cross product of the X and Z vectors ("up" vector)
	uY = uZ.cross(uX);
	
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

void camera::setPerspectiveProjection(const float& nearPlane, const float& farPlane) {
	projection = Matrix4::getPerspectiveMatrix(fov, A, nearPlane, farPlane);
}

void camera::setOrthographicProjection(const float& nearPlane, const float& farPlane) {
	projection = Matrix4::getOrthographicMatrix(fov, A, nearPlane, farPlane);
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

bool camera::checkCulling(const Vector3 &offset, const triangle &tri){
	// Check if the triangle is facing towards the camera
	return ((tri.getCenterPoint() - position) * tri.getNormal() <= 0);
}

bool camera::projectPoint(const Vector3 &vertex, float& sX, float& sY, float* zdepth/*=0x0*/){
	Vector3 vec = position - vertex;
	ray proj(vertex, vec);
	
	if (zdepth) {
		(*zdepth) = -(vec.length() * vec.cosTheta(uZ));
		if (*zdepth < 0) // Vertex behind camera
			return false;
	}

	float t;
	if(intersects(proj, t)){
		// Get the point at which the ray intersect the viewing plane
		Vector3 pp = proj.extend(t) - p;

		// Convert the viewing plane point to screen space (-1, 1)
		convertToScreenSpace(pp, sX, sY);
		
		return true;
	}
	
	return false;
}

float camera::getZDepth(const Vector3& p) const {
	Vector3 zDepth = p - position; // Vector from focal point of camera to center of polygon
	return (zDepth.length() * zDepth.cosTheta(uZ));
}

void camera::dump() const {
	std::cout << " FOV:    " << fov*180/pi << " degrees\n";
	std::cout << " f:      " << L << " m\n";
	std::cout << " A:      " << A << "\n";
	std::cout << " Width:  " << W << " m\n";
	std::cout << " Height: " << H << " m\n";
	std::cout << " unitX = (" << uX[0] << ", " << uX[1] << ", " << uX[2] << ")\n";
	std::cout << " unitY = (" << uY[0] << ", " << uY[1] << ", " << uY[2] << ")\n";
	std::cout << " unitZ = (" << uZ[0] << ", " << uZ[1] << ", " << uZ[2] << ")\n";
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
	// Update the OpenGL projection matrix
	setPerspectiveProjection(0.1f, 100.f);
}

void camera::updateViewingPlane(){
	p = position + uZ*L;
	norm = uZ;
}

void camera::resetUnitVectors() {
	uX = unitVectorX;
	uY = unitVectorY;
	uZ = unitVectorZ;
}

void camera::convertToScreenSpace(const Vector3 &vec, float& x, float& y){
	x = 2*(vec * uX)/W; // unitless
	y = 2*(vec * uY)/H; // unitless
}

bool camera::rayTrace(const ray& cast, const pixelTriplet& pixels, Vector3& P) const {
	float t;
	if(pixels.intersects(cast, t)){
		P = cast.extend(t);
		return true;
	}
	return false;
}
