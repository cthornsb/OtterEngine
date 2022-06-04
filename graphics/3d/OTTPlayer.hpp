#ifndef OTT_PLAYER_HPP
#define OTT_PLAYER_HPP

#include <graphics/geometry/OTTObject.hpp>
#include <math/OTTMatrix.hpp>
#include <math/OTTVector.hpp>

namespace ott {

class Camera;

class Player : public Object {
public:
	Player() :
		Object(),
		fMass(1.f),
		vForward(0, 0, 1.f), // Looking along the +Z axis
		velocity(),
		acceleration(),
		vEyesOffset(),
		eyes(0x0)
	{
	}

	Player(const Object& obj, const Vector3& offset) :
		Object(obj),
		fMass(1.f),
		vForward(0, 0, 1.f), // Looking along the +Z axis
		velocity(),
		acceleration(),
		vEyesOffset(offset),
		eyes(0x0)
	{
	}

	Vector3 EyePosition() const {
		return (position + vEyesOffset);
	}

	void SetEyeOffset(const Vector3& offset);

	void SetEyes(Camera* cam);

	/** Set the mass of the player (in kg)
	  */
	void SetMass(const float& mass) {
		fMass = mass;
	}

	void Look(const float& pitch, const float& yaw);

	/** Set the position of the player
	  */
	void Warp(const Vector3& pos);

	/** Move the player a distance through 3d space
	  */
	void Move(const Vector3& displacement);

	/** Move the player in the XZ-plane
	  */
	void Move(const float& forward, const float& strafe);

	/**
	  */
	void Move(const float& forward);

	/** Strafe to the left or right
	  */
	void Strafe(const float& strafe);

	/** Rotate the actor object by a given amount about the Y axis (in radians)
	  */
	void Rotate(const float& theta);

	/** Apply an instantaneous impulse to the player, instantly accelerating it to a specified velocity (i.e. give it a momentum).
	  * The player's acceleration is not affected.
	  */
	void ApplyImpulse(const Vector3& momentum);

	/** Apply a force to the player, changing its acceleration vector accordingly
	  */
	void ApplyForce(const Vector3& force);

	/** Update player kinematics
	  */
	void Update(const float& timeStep);

private:
	float fMass; ///< Player's mass (in kg)

	Vector3 vForward; ///< Forward movement direction vector

	Vector3 vEyesOffset; 

	Vector3 velocity; ///< Player's velocity vector (in m/s)

	Vector3 acceleration; ///< Player's acceleration vector (in m/s^2)

	Matrix2 actorRotation;

	Vector3 uActorX;

	Vector3 uActorZ;

	Camera* eyes;
};

} /* namespace ott */

#endif // ifndef OTT_PLAYER_HPP