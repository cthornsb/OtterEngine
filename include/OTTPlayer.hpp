#ifndef OTT_PLAYER_HPP
#define OTT_PLAYER_HPP

#include "object.hpp"
#include "Vector.hpp"

class camera;

class OTTPlayer : public object {
public:
	OTTPlayer() :
		object(),
		fMass(1.f),
		vForward(0, 0, 1.f), // Looking along the +Z axis
		velocity(),
		acceleration(),
		vEyesOffset(),
		eyes(0x0)
	{
	}

	OTTPlayer(const object& obj, const Vector3& offset) :
		object(obj),
		fMass(1.f),
		vForward(0, 0, 1.f), // Looking along the +Z axis
		velocity(),
		acceleration(),
		vEyesOffset(offset),
		eyes(0x0)
	{
	}

	void setEyeOffset(const Vector3& offset);

	void setEyes(camera* cam);

	/** Set the mass of the player (in kg)
	  */
	void setMass(const float& mass) {
		fMass = mass;
	}

	void look(const float& pitch, const float& yaw);

	/** Set the position of the player
	  */
	void warp(const Vector3& pos);

	/** Move the player a distance through 3d space
	  */
	void move(const Vector3& displacement);

	/** Move the player in the XZ-plane
	  */
	void move(const float& forward, const float& strafe);

	/**
	  */
	void move(const float& forward);

	/** Strafe to the left or right
	  */
	void strafe(const float& strafe);

	/** Rotate the actor object by a given amount about the Y axis (in radians)
	  */
	void rotate(const float& theta);

	/** Apply an instantaneous impulse to the player, instantly accelerating it to a specified velocity (i.e. give it a momentum).
	  * The player's acceleration is not affected.
	  */
	void applyImpulse(const Vector3& momentum);

	/** Apply a force to the player, changing its acceleration vector accordingly
	  */
	void applyForce(const Vector3& force);

	/** Update player kinematics
	  */
	void update(const float& timeStep);

private:
	float fMass; ///< Player's mass (in kg)

	Vector3 vForward; ///< Forward movement direction vector

	Vector3 vEyesOffset; 

	Vector3 velocity; ///< Player's velocity vector (in m/s)

	Vector3 acceleration; ///< Player's acceleration vector (in m/s^2)

	Matrix2 actorRotation;

	Vector3 uActorX;

	Vector3 uActorZ;

	camera* eyes;
};

#endif // ifndef OTT_PLAYER_HPP