#include "OTTCamera.hpp"

#include <graphics/geometry/OTTObject.hpp>
#include <graphics/geometry/OTTVertex.hpp>
#include <graphics/geometry/OTTPolygon.hpp>
#include <math/OTTConstants.hpp>
#include <math/OTTMatrix.hpp>

#include <iostream>
#include <string>

ott::Camera::Camera() :
	Plane(),
	fov(constants::kPi / 2), // radians
	L(50E-3f), // m
	A(640.0f/480), 
	W(0),
	H(0),
	position(),
	uX(1,0,0),
	uY(0,1,0),
	uZ(0,0,1),
	pitchAngle(0, -constants::kPi / 2, constants::kPi / 2, true),
	rollAngle(0, -constants::kPi / 2, constants::kPi / 2, true),
	yawAngle(0, -constants::kPi, constants::kPi)
{ 
	this->Initialize();
}

ott::Camera::Camera(const Vector3 &pos_) :
	Camera()
{ 
	position = pos_;
	this->Initialize();
}

ott::Camera::Camera(const Vector3 &pos_, const Vector3 &dir_) :
	Camera() 
{
	position = pos_;
	this->Initialize();
	this->SetDirection(dir_);
}

ott::Camera::~Camera(){
}

ott::Matrix4* ott::Camera::ViewMatrix() {
	// Update the view matrix as the camera may have moved
	viewMatrix.SetViewMatrixFPS(position, rotation); // Construct view matrix from position and rotation matrix
	return &viewMatrix;
}

void ott::Camera::SetFOV(const float &fov_){ 
	fov = fov_ * constants::kDeg2rad;
	this->ComputeViewingPlane();
}

void ott::Camera::SetFocalLength(const float &length){
	L = length * 1E-3f;
	this->ComputeViewingPlane();
}

void ott::Camera::SetAspectRatio(const float &ratio){
	A = ratio;
	this->ComputeViewingPlane();
}

/////////////////////////////////////////////////
// Movement methods
/////////////////////////////////////////////////

void ott::Camera::MoveForward(const float &dist){
	position += uZ*dist;
	this->UpdateViewingPlane();
}

void ott::Camera::MoveRight(const float&dist){
	position += uX*dist;
	this->UpdateViewingPlane();
}

void ott::Camera::MoveUp(const float&dist){
	position += uY*dist;
	this->UpdateViewingPlane();	
}

void ott::Camera::Move(const Vector3 &displacement){
	position += displacement;
	this->UpdateViewingPlane();
}

void ott::Camera::Move(const float&x, const float&y, const float&z){
	position += Vector3(x, y, z);
	this->UpdateViewingPlane();
}

void ott::Camera::MoveTo(const Vector3 &pos){ 
	position = pos;
	this->UpdateViewingPlane();
}

void ott::Camera::MoveTo(const float&x, const float&y, const float&z){
	position = Vector3(x, y, z);
	this->UpdateViewingPlane();
}

/////////////////////////////////////////////////////////////////////
// Rotation methods
/////////////////////////////////////////////////////////////////////

void ott::Camera::Rotate(const float& pitch, const float& yaw, const float& roll){
	// Rotate the camera relative to its current orientation
	this->SetRotation((pitchAngle += pitch), (yawAngle += yaw), (rollAngle += roll));
}

void ott::Camera::SetRotation(const float& pitch, const float& yaw, const float& roll){
	// Get the rotation matrix
	rotation.SetRotation(pitch, roll, yaw);

	// Update the rotation of the three unit vectors
	uX = rotation.Transform(constants::kUnitVectorX);
	uY = rotation.Transform(constants::kUnitVectorY);
	uZ = rotation.Transform(constants::kUnitVectorZ);
	uZ.Invert(); // Necessary in OpenGL render mode

	// Update the viewing plane
	this->UpdateViewingPlane();
}

void ott::Camera::RotateFPS(const float& pitch, const float& yaw) {
	// Rotate the camera relative to its current orientation
	this->SetRotationFPS((pitchAngle += pitch), (yawAngle += yaw));
}

void ott::Camera::SetRotationFPS(const float& pitch, const float& yaw) {
	// Get the rotation matrix
	rotation = Matrix3::FPSCameraMatrix((pitchAngle = pitch), (yawAngle = yaw));

	// Update the rotation of the three unit vectors
	uX = rotation.Transform(constants::kUnitVectorX);
	uY = rotation.Transform(constants::kUnitVectorY);
	uZ = rotation.Transform(constants::kUnitVectorZ);
	uZ.Invert(); // Necessary in OpenGL render mode

	// Update the viewing plane
	this->UpdateViewingPlane();
}

void ott::Camera::SetDirection(const Vector3& dir, const Vector3& upVector/* = unitVectorY*/) {
	this->LookAt(position + dir, upVector);
}

void ott::Camera::LookAt(const Vector3 &position, const Vector3& upVector/* = unitVectorY*/){
	// Point the Z axis unit vector at the point in space
	uZ = (position - position).Normalize();
	uZ.Invert(); // Necessary in OpenGL render mode

	// Compute the X-axis unit vector ("right" vector)
	uX = constants::kUnitVectorY.Cross(uZ).Normalize();
		
	// Get the Y-axis unit vector by computing the cross product of the X and Z vectors ("up" vector)
	uY = uZ.Cross(uX);
	
	// Update the viewing plane
	this->UpdateViewingPlane();
}

void ott::Camera::ResetOrientation(){
	this->ResetUnitVectors();
	this->UpdateViewingPlane();
	pitchAngle = 0;
	rollAngle = 0;
	yawAngle = 0;
}

void ott::Camera::SetPerspectiveProjection(const float& nearPlane, const float& farPlane) {
	projection = Matrix4::PerspectiveMatrix(fov, A, nearPlane, farPlane);
}

void ott::Camera::SetOrthographicProjection(const float& nearPlane, const float& farPlane) {
	projection = Matrix4::OrthographicMatrix(fov, A, nearPlane, farPlane);
}

/////////////////////////////////////////////////
// Rendering methods
/////////////////////////////////////////////////

#if defined(SOFTWARE_RENDERER)

bool ott::Camera::Render(pixelTriplet& pixels){
	bool retval = true;
	retval &= projectPoint(pixels[0]->getPosition(), pixels[0]->sX, pixels[0]->sY, &pixels[0]->zDepth);
	retval &= projectPoint(pixels[1]->getPosition(), pixels[1]->sX, pixels[1]->sY, &pixels[1]->zDepth);
	retval &= projectPoint(pixels[2]->getPosition(), pixels[2]->sX, pixels[2]->sY, &pixels[2]->zDepth);
	return retval;
}

bool ott::Camera::RayTrace(const Ray_t& cast, const pixelTriplet& pixels, Vector3& P) const {
	float t;
	if(pixels.intersects(cast, t)){
		P = cast.extend(t);
		return true;
	}
	return false;
}

bool ott::Camera::Render(Vertex& vertex) {
	return this->ProjectPoint(vertex.Position(), vertex.sX, vertex.sY, &vertex.zDepth);
}

bool ott::Camera::CheckCulling(const Vector3 &offset, const Polygon &tri){
	// Check if the triangle is facing towards the camera
	return ((tri.CenterPoint() - position) * tri.Normal() <= 0);
}

bool ott::Camera::ProjectPoint(const Vector3 &vertex, float& sX, float& sY, float* zdepth/*=0x0*/){
	Vector3 vec = position - vertex;
	Ray_t proj(vertex, vec);
	
	if (zdepth) {
		(*zdepth) = -(vec.Length() * vec.CosTheta(uZ));
		if (*zdepth < 0) // Vertex behind camera
			return false;
	}

	float t;
	if(this->Intersects(proj, t)){
		// Get the point at which the ray intersect the viewing plane
		Vector3 pp = proj.Extend(t) - pos;

		// Convert the viewing plane point to screen space (-1, 1)
		this->ConvertToScreenSpace(pp, sX, sY);
		
		return true;
	}
	
	return false;
}

float ott::Camera::ZDepth(const Vector3& p) const {
	Vector3 zDepth = p - position; // Vector from focal point of camera to center of polygon
	return (zDepth.Length() * zDepth.CosTheta(uZ));
}

#endif // #if defined(SOFTWARE_RENDERER)

void ott::Camera::Dump() const {
	std::cout << " FOV:    " << fov * constants::kRad2deg << " degrees\n";
	std::cout << " f:      " << L << " m\n";
	std::cout << " A:      " << A << "\n";
	std::cout << " Width:  " << W << " m\n";
	std::cout << " Height: " << H << " m\n";
	std::cout << " unitX = (" << uX[0] << ", " << uX[1] << ", " << uX[2] << ")\n";
	std::cout << " unitY = (" << uY[0] << ", " << uY[1] << ", " << uY[2] << ")\n";
	std::cout << " unitZ = (" << uZ[0] << ", " << uZ[1] << ", " << uZ[2] << ")\n";
}

void ott::Camera::Initialize(){
	// Set initial parameters
	this->ResetUnitVectors();

	// Setup the viewing plane (looking down the Z-axis)
	this->UpdateViewingPlane();

	// Compute the dimensions of the viewing plane
	this->ComputeViewingPlane();
}

void ott::Camera::ComputeViewingPlane(){
	W = 2*L*std::tan(fov/2); // m
	H = W/A; // m
	// Update the OpenGL projection matrix
	this->SetPerspectiveProjection(0.1f, 10.f);
}

void ott::Camera::UpdateViewingPlane(){
	pos = position + uZ*L;
	norm = uZ;
}

void ott::Camera::ResetUnitVectors() {
	uX = constants::kUnitVectorX;
	uY = constants::kUnitVectorY;
	uZ = constants::kUnitVectorZ;
}

void ott::Camera::ConvertToScreenSpace(const Vector3 &vec, float& x, float& y){
	x = 2*(vec * uX)/W; // unitless
	y = 2*(vec * uY)/H; // unitless
}
