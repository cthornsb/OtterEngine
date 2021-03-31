#include "OTTPlayer.hpp"

#include "camera.hpp"

void OTTPlayer::setEyeOffset(const Vector3& offset) {
	vEyesOffset = offset;
	eyes->moveTo(position + vEyesOffset);
}

void OTTPlayer::setEyes(camera* cam) {
	eyes = cam;
	eyes->moveTo(position + vEyesOffset);
}

void OTTPlayer::look(const float& pitch, const float& yaw) {
	// Rotate the camera relative to its current orientation
	rotate(yaw);
	eyes->rotateFPS(pitch, yaw);
}

void OTTPlayer::warp(const Vector3& pos) {
	position = pos;
	eyes->moveTo(position + vEyesOffset);
	updatePosition();
}

void OTTPlayer::move(const Vector3& displacement) {
	position += displacement;
	eyes->moveTo(position + vEyesOffset);
	updatePosition();
}

void OTTPlayer::move(const float& forward, const float& strafe) {
	position += (uZ * (-forward) + uX * (-strafe));
	eyes->moveTo(position + vEyesOffset);
	updatePosition();
}

void OTTPlayer::move(const float& forward) {
	position += (uZ * (-forward));
	eyes->moveTo(position + vEyesOffset);
	updatePosition();
}

void OTTPlayer::strafe(const float& strafe) {
	position += (uX * (-strafe));
	eyes->moveTo(position + vEyesOffset);
	updatePosition();
}

void OTTPlayer::rotate(const float& theta) {
	rotation = Matrix3::getYawMatrix(yawAngle += theta);
	updateRotation();
}

void OTTPlayer::applyImpulse(const Vector3& momentum) {
	velocity = momentum * (1.f / fMass);
}

void OTTPlayer::applyForce(const Vector3& force) {
	acceleration += force * (1.f / fMass);
}

void OTTPlayer::update(const float& timeStep) {
	move(velocity * timeStep);
	velocity += (acceleration * timeStep);
	if (position[1] <= 0.f) // For testing
		velocity[1] = 0.f;
}