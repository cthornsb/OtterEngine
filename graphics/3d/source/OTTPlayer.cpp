#include "OTTPlayer.hpp"
#include "OTTCamera.hpp"

void ott::Player::SetEyeOffset(const Vector3& offset) {
	vEyesOffset = offset;
	eyes->MoveTo(position + vEyesOffset);
}

void ott::Player::SetEyes(Camera* cam) {
	eyes = cam;
	eyes->MoveTo(position + vEyesOffset);
}

void ott::Player::Look(const float& pitch, const float& yaw) {
	// Rotate the camera relative to its current orientation
	this->Rotate(yaw);
	eyes->RotateFPS(pitch, yaw);
}

void ott::Player::Warp(const Vector3& pos) {
	position = pos;
	eyes->MoveTo(position + vEyesOffset);
}

void ott::Player::Move(const Vector3& displacement) {
	position += displacement;
	eyes->MoveTo(position + vEyesOffset);
}

void ott::Player::Move(const float& forward, const float& strafe) {
	position += (uZ * (-forward) + uX * (-strafe));
	eyes->MoveTo(position + vEyesOffset);
}

void ott::Player::Move(const float& forward) {
	position += (uZ * (-forward));
	eyes->MoveTo(position + vEyesOffset);
}

void ott::Player::Strafe(const float& strafe) {
	position += (uX * (-strafe));
	eyes->MoveTo(position + vEyesOffset);
}

void ott::Player::Rotate(const float& theta) {
	rotation = Matrix3::YawMatrix(yawAngle += theta);
	this->UpdateRotation();
}

void ott::Player::ApplyImpulse(const Vector3& momentum) {
	velocity = momentum * (1.f / fMass);
}

void ott::Player::ApplyForce(const Vector3& force) {
	acceleration += force * (1.f / fMass);
}

void ott::Player::Update(const float& timeStep) {
	this->Move(velocity * timeStep);
	velocity += (acceleration * timeStep);
	if (position[1] <= 0.f) { // todo For testing
		velocity[1] = 0.f;
	}
}
