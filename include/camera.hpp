#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "ray.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "PixelTriplet.hpp"
#include "WrappedValue.hpp"
#include "Matrix.hpp"

/** @class camera
  * @brief Handles all projections of 3d geometry onto the screen
  * @author Cory R. Thornsberry
  * @date September 4, 2019
  */

class camera : public plane {
public:
	/** Default constructor
	  */
	camera();
	
	/** Camera position constructor
	  */
	camera(const Vector3 &pos_);

	/** Camera position and rotation constructor
	  */
	camera(const Vector3 &pos_, const Vector3 &dir_);

	/** Destructor
	  */
	~camera();

	/** Get the current position of the focal point of the camera
	  */
	Vector3 getPosition() const { return pos; }

	/** Get the current direction the camera is facing
	  */
	Vector3 getDirection() const { return uZ; }

	/** Get a pointer to the OpenGL scene projection matrix
	  */
	Matrix4* getProjectionMatrix() { return &projection; }

	/** Update the OpenGL camera view matrix and return its pointer
	  */
	Matrix4* getViewMatrix();

	/** Get the pitch-angle of the camera (about the x-axis, in radians)
	  */
	float getPitchAngle() const { return pitchAngle.get(); }

	/** Get the yaw-angle of the camera (about the y-axis, in radians)
	  */
	float getYawAngle() const { return yawAngle.get(); }

	/** Get the roll-angle of the camera (about the z-axis, in radians)
	  */
	float getRollAngle() const { return rollAngle.get(); }

	/** Get a ray which originates at the focal point and travels through the viewplane
	  * @param x Horizontal coordinate in screen space (-1, 1)
	  * @param y Vertical coordinate in screen space (-1, 1)
	  */
	ray getRay(const float& x, const float& y) const { return ray(pos, (p + uX * x + uY * y - pos).normalize()); }

	/** Set the field-of-view of the camera (in degrees)
	  */
	void setFOV(const float &fov_);
	
	/** Set the focal-length of the camera (in mm)
	  */
	void setFocalLength(const float &length);
	
	/** Set the aspect ratio of the screen
	  */
	void setAspectRatio(const float &ratio);

/////////////////////////////////////////////////
// Movement methods
/////////////////////////////////////////////////

	/** Move the camera in the direction that it is currently pointed
	  */
	void moveForward(const float& dist);

	/** Move the camera in the direction that it is currently pointed
	  */
	void moveBackward(const float& dist){ moveForward(-dist); }

	/** Move the camera horizontally along the axis perpendicular to the direction it is facing
	  */
	void moveRight(const float& dist);

	/** Move the camera horizontally along the axis perpendicular to the direction it is facing
	  */
	void moveLeft(const float& dist){ moveRight(-dist); }

	/** Move the camera vertically along the axis perpendicular to the direction it is facing
	  */	
	void moveUp(const float& dist);
	
	/** Move the camera vertically along the axis perpendicular to the direction it is facing
	  */
	void moveDown(const float& dist){ moveUp(-dist); }
	
	/** Move the camera along a vector from its current position
	  */
	void move(const Vector3 &displacement);
	
	/** Move the camera along the X, Y, and Z axes from its current position
	  */
	void move(const float& x, const float& y, const float& z);
	
	/** Move the camera to a position in 3d space
	  */
	void moveTo(const Vector3 &position);
	
	/** Move the camera to a position in 3d space
	  */
	void moveTo(const float& x, const float& y, const float& z);

/////////////////////////////////////////////////
// Rotation methods
/////////////////////////////////////////////////

	/** Rotate the camera by a given amount using the pitch-yaw-roll convention (all in radians)
	  * @note This method rotates the camera relative to its current rotation. Use setRotation() to specify the rotation explicitly
	  * @param pitch Angle to tilt the camera up or down (i.e. about the horizontal axis)
	  * @param yaw Angle to turn the camera left or right (i.e. about the vertical axis)
	  * @param roll Angle to roll the camera (i.e. about the axis into the screen)
	  */
	void rotate(const float& pitch, const float& yaw, const float& roll);

	/** Rotate the object to specified angles using the pitch-roll-yaw convention (all in radians)
	  * @param pitch Angle to tilt the camera up or down (i.e. about the horizontal axis)
	  * @param yaw Angle to turn the camera left or right (i.e. about the vertical axis)
	  * @param roll Angle to roll the camera (i.e. about the axis into the screen)
	  */
	void setRotation(const float& pitch, const float& yaw, const float& roll);

	/** Rotate the camera by a given angle about the x-axis (pitch) and then about the y-axis (yaw) to
	  * simulate a "FPS" style camera. This method rotates the camera relative to its current rotation.
	  * Use setRotationFPS() to specify the rotation explicitly.
	  * @param pitch Angle to tilt the camera up or down (i.e. about the horizontal axis)
	  * @param yaw Angle to turn the camera left or right (i.e. about the vertical axis)
	  */
	void rotateFPS(const float& pitch, const float& yaw);

	/** Rotate the camera by a given angle about the x-axis (pitch) and then about the y-axis (yaw) to
	  * simulate a "FPS" style camera. This method rotates the camera relative to its current rotation.
	  * @param pitch Angle to tilt the camera up or down (i.e. about the horizontal axis)
	  * @param yaw Angle to turn the camera left or right (i.e. about the vertical axis)
	  */
	void setRotationFPS(const float& pitch, const float& yaw);

	/** Set the direction of the camera
	  * @param dir Direction of camera in 3d space
	  * @param upVector The vertical vector for camera orientation (+Y by default)
	  */
	void setDirection(const Vector3& dir, const Vector3& upVector = unitVectorY);

	/** Point the camera at a location in 3d space
	  * @param position The 3d point the camera is pointed at
	  * @param upVector The vertical vector for camera orientation (+Y by default)
	  */
	void lookAt(const Vector3& position, const Vector3& upVector = unitVectorY);

	/** Reset the orientation of the camera to its default rotation
	  */
	void resetOrientation();

	/** Set the OpenGL projection matrix to perspective mode
	  * @param nearPlane The distance from the focal point to the near clipping plane
	  * @param farPlane The distance from the focal point to the far clipping plane
	  */
	void setPerspectiveProjection(const float& nearPlane, const float& farPlane);

	/** Set the OpenGL projection matrix to orthographic mode
	  * @param nearPlane The distance from the focal point to the near clipping plane
	  * @param farPlane The distance from the focal point to the far clipping plane
	  */
	void setOrthographicProjection(const float& nearPlane, const float& farPlane);

/////////////////////////////////////////////////
// Rendering methods
/////////////////////////////////////////////////

	/** Trace a ray, from the camera position, through a point on the viewing plane to the surface-plane of an input triangle
	  * @param ray The 3d ray which will be cast
	  * @param tri The triangle whose surfac will be intersected by the ray
	  * @param P The point (in real-space) where the ray intersects the surface-plane of the triangle
	  * @return True if the ray intersects with the surface-plane of the triangle, and return false otherwise
	  */
	bool rayTrace(const ray& cast, const pixelTriplet& pixels, Vector3& P) const;

	/** Render a single triangle by computing its projection onto the viewing plane
	  * @param pixels Storage object for triangle rendering data
	  * @return True if any of the three vertices are visible and return false otherwise
	  */
	bool render(pixelTriplet& pixels);

	/** Render a single vertex by computing its projection onto the viewing plane
	  * @param vertex Storage object for vertex rendering data
	  * @return True if the vertex is visible and return false otherwise
	  */
	bool render(Vertex& vertex);

	/** Check whether or not a triangle is facing towards the camera
	  * @param offset The offset of the object from the world origin
	  * @param tri The triangles to draw
	  * @return True if the triangle is facing towards the camera and return false otherwise
	  */
	bool checkCulling(const Vector3 &offset, const triangle &tri);

	/** Compute the coordinates where a ray from a vertex intersects the viewing plane
	  * @param vertex The originating point of the ray (in real-space)
	  * @param sX The horizontal component of the position where the ray intersects the viewing plane (in screen-space)
	  * @param sY The vertical component of the position where the ray intersects the viewing plane (in screen-space)
	  * @param zdepth Computed z-depth into the screen (not used if pointer is null)
	  * @return True if the ray intersects the viewing plane, and return false otherwise
	  */
	bool projectPoint(const Vector3 &vertex, float& sX, float& sY, float* zdepth=0x0);

	/** Get the depth into the screen (i.e. distance along the z-axis of the camera) for a point in 3d space
	  * @return The distance into the screen to the specified point, or out of the screen in the event that the point is behind the camera
	  */
	float getZDepth(const Vector3& p) const ;

	/** Dump camera parameters to stdout
	  */
	void dump() const ;
	
private:
	float fov; ///< Field-of-view (in radians)
	float L; ///< Focal length (in m)
	float A; ///< Aspect ratio
	float W; ///< Viewing plane width (in m)
	float H; ///< Viewing plane height (in m)
	
	Vector3 pos; ///< The focal point of the camera (its position)

	Vector3 uX; ///< Unit vector for the x-axis
	Vector3 uY; ///< Unit vector for the y-axis
	Vector3 uZ; ///< Unit vector for the z-axis

	WrappedValue pitchAngle; ///< Angle of camera tilt up or down (i.e. about the horizontal axis)
	WrappedValue rollAngle; ///< Angle of camera roll cw or ccw (i.e. about the depth axis)
	WrappedValue yawAngle; ///< Angle of camera turn left or right (i.e. about the vertical axis

	Matrix3 rotation; ///< 3d camera rotation matrix

	Matrix4 projection; ///< Camera projection matrix (perspective or orthographic)
	Matrix4 viewMatrix; ///< Camera view matrix (rotation and translation)

	/** Initialize the camera by setting initial values and computing all geometric parameters
	  */
	void initialize();
	
	/** Compute the dimensions of the viewing plane based on the field-of-view, the focal-length,
	  * and the aspect-ratio
	  */
	void computeViewingPlane();
	
	/** Update the central point and normal vector of the viewing plane
	  */
	void updateViewingPlane();

	/** Reset unit vectors to the identity matrix
	  */
	void resetUnitVectors();
	
	/** Convert a real-space position on the viewing plane to a position in screen-space [-1, 1]
	  * @note The origin of screen-space is the center of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the top
	  * @param vec The position on the surface of the viewing plane (in real-space)
	  * @param x The horizontal component of the screen-space coordinate
	  * @param y The vertical component of the screen-space coordinate
	  */
	void convertToScreenSpace(const Vector3 &vec, float& x, float& y);
};

#endif
