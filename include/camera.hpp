#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <vector>

#include "ray.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "colors.hpp"
#include "lightSource.hpp"

extern const double pi;
extern const double deg2rad;
extern const double rad2deg;

class object;

/** @class camera
  * @brief Handles all projections of 3d geometry onto the screen
  * @author Cory R. Thornsberry
  * @date September 4, 2019
  */

class camera {
public:
	/** Default constructor
	  */
	camera();
	
	/** Camera position constructor
	  */
	camera(const vector3 &pos_);

	/** Camera position and rotation constructor
	  */
	camera(const vector3 &pos_, const vector3 &dir_);

	/** Destructor
	  */
	~camera();

	/** Set the field-of-view of the camera (in degrees)
	  */
	void setFOV(const double &fov_);
	
	/** Set the focal-length of the camera (in mm)
	  */
	void setFocalLength(const double &length);
	
	/** Set the aspect ratio of the screen
	  */
	void setAspectRatio(const double &ratio);

/////////////////////////////////////////////////
// Movement methods
/////////////////////////////////////////////////

	/** Move the camera in the direction that it is currently pointed
	  */
	void moveForward(const double &dist);

	/** Move the camera in the direction that it is currently pointed
	  */
	void moveBackward(const double &dist){ moveForward(-dist); }

	/** Move the camera horizontally along the axis perpendicular to the direction it is facing
	  */
	void moveRight(const double &dist);

	/** Move the camera horizontally along the axis perpendicular to the direction it is facing
	  */
	void moveLeft(const double &dist){ moveRight(-dist); }

	/** Move the camera vertically along the axis perpendicular to the direction it is facing
	  */	
	void moveUp(const double &dist);
	
	/** Move the camera vertically along the axis perpendicular to the direction it is facing
	  */
	void moveDown(const double &dist){ moveUp(-dist); }
	
	/** Move the camera along a vector from its current position
	  */
	void move(const vector3 &displacement);
	
	/** Move the camera along the X, Y, and Z axes from its current position
	  */
	void move(const double &x, const double &y, const double &z);
	
	/** Move the camera to a position in 3d space
	  */
	void moveTo(const vector3 &position);
	
	/** Move the camera to a position in 3d space
	  */
	void moveTo(const double &x, const double &y, const double &z);

/////////////////////////////////////////////////
// Rotation methods
/////////////////////////////////////////////////

	/** Rotate the object by a given amount using the pitch-roll-yaw convention (all in radians)
	  * @note This method will rotate vertices from their current position. Use setRotation() to specify the rotation explicitly
	  */
	void rotate(const double &theta, const double &phi, const double &psi);

	/** Rotate the object to specified angles using the pitch-roll-yaw convention (all in radians)
	  */
	void setRotation(const double &theta, const double &phi, const double &psi);

	/** Point the camera at a location in 3d space
	  */
	void lookAt(const vector3 &position);

	/** Reset the orientation of the camera to its default rotation
	  */
	void resetRotation();

/////////////////////////////////////////////////
// Rendering methods
/////////////////////////////////////////////////

	/** Render a single triangle by computing its projection onto the viewing plane
	  * @param offset The offset of the object from the world origin
	  * @param tri The triangles to render
	  * @param pixelX Array of horizontal coordinates for the three vertices (must contain at least 3 elements)
	  * @param pixelY Array of vertical coordinates for the three vertices (must contain at least 3 elements)
	  */
	void render(const vector3 &offset, const triangle &tri, double *sX, double *sY);

	/** Check whether or not a triangle is facing towards the camera
	  * @param offset The offset of the object from the world origin
	  * @param tri The triangles to draw
	  * @return True if the triangle is facing towards the camera and return false otherwise
	  */
	bool checkCulling(const vector3 &offset, const triangle &tri);

	/** Compute the coordinates where a ray from a vertex intersects the viewing plane
	  * @param vertex The originating point of the ray (in real-space)
	  * @param sX The horizontal component of the position where the ray intersects the viewing plane (in screen-space)
	  * @param sY The vertical component of the position where the ray intersects the viewing plane (in screen-space)
	  * @return True if the ray intersects the viewing plane, and reutrn false otherwise
	  */
	bool projectPoint(const vector3 &vertex, double &sX, double &sY);

	/** Dump camera parameters to stdout
	  */
	void dump() const ;
	
private:
	double fov; ///< Field-of-view (in radians)
	double L; ///< Focal length (in m)
	double A; ///< Aspect ratio
	double W; ///< Viewing plane width (in m)
	double H; ///< Viewing plane height (in m)

	plane vPlane; ///< The viewing plane of the camera
	
	vector3 pos; ///< The focal point of the camera (its position)
	
	vector3 uX; ///< Unit vector for the x-axis
	vector3 uY; ///< Unit vector for the y-axis
	vector3 uZ; ///< Unit vector for the z-axis
	
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
	
	/** Convert a real-space position on the viewing plane to a position in screen-space [-1, 1]
	  * @note The origin of screen-space is the center of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the top
	  * @param vec The position on the surface of the viewing plane (in real-space)
	  * @param x The horizontal component of the screen-space coordinate
	  * @param y The vertical component of the screen-space coordinate
	  */
	void convertToScreenSpace(const vector3 &vec, double &x, double &y);
	
	/** Trace a ray, from the camera position, through a point on the viewing plane to the surface-plane of an input triangle
	  * @param sX The x-coordinate on the viewing plane through which the ray will be cast (in screen-space)
	  * @param sY The y-coordinate on the viewing plane through which the ray will be cast (in screen-space)
	  * @param tri The triangle whose surface-plane will be intersected by the ray
	  * @param P The point (in real-space) where the ray intersects the surface-plane of the triangle
	  * @return True if the ray intersects with the surface-plane of the triangle, and return false otherwise
	  */
	bool rayTrace(const double &sX, const double &sY, const triangle &tri, vector3 &P);
};

#endif
