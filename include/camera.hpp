#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <vector>

#include "ray.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "colors.hpp"

extern const double pi;
extern const double deg2rad;
extern const double rad2deg;

class sdlWindow;

/** @class camera
  * @brief Handles all projections of 3d geometry onto the screen
  * @author Cory R. Thornsberry
  * @date September 4, 2019
  */

class camera : public ray {
public:
	/** Supported drawing modes
	  */
	enum drawMode {WIREFRAME, ///< Draw outlines of triangles with no backface culling
	               MESH,      ///< Draw outlines of triangles with backface culling
	               SOLID,     ///< Draw filled triangles with wireframe outline overlayed
	               RENDER     ///< Draw filled triangles with light shading
	};

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

	/** Get the width of the screen (in pixels)
	  */
	int getScreenWidth() const { return screenWidthPixels; }

	/** Get the height of the screen (in pixels)
	  */	
	int getScreenHeight() const { return screenHeightPixels; }

	/** Set the width of the screen (in pixels)
	  */
	void setScreenWidth(const int &width){ screenWidthPixels = width; }
	
	/** Set the height of the screen (in pixels)
	  */
	void setScreenHeight(const int &height){ screenHeightPixels = height; }

	/** Enable or disable the drawing of triangle normals
	  */
	void setDrawNormals(const bool &enable=true){ drawNorm = enable; }
	
	/** Move the camera in the direction that it is currently pointed
	  * @param dist The distance to move along the direction the camera is currently pointed
	  */
	void moveCam(const double &dist);

	/** Render a vector of triangles in 3d space
	  * @param tri Pointer to the vector of triangles to draw
	  * @param mode The drawing mode to use
	  */
	void draw(std::vector<triangle> *tri, const drawMode &mode=WIREFRAME);
	
	/** Render a single triangle in 3d space
	  * @param tri The triangles to draw
	  * @param mode The drawing mode to use
	  */
	void draw(const triangle &tri, const drawMode &mode=WIREFRAME);

	/** Clear the screen by filling it with a color (black by default)
	  */
	void clear(const sdlColor &color=Colors::BLACK);
	
	/** Render the current screen
	  */
	void render();

	/** Wait until the user closes the screen, and then return
	  */
	void loop();

	/** Dump camera parameters to stdout
	  */
	void dump() const ;
	
private:
	double fov; ///< Field-of-view (in radians)
	double L; ///< Focal length (in m)
	double A; ///< Aspect ratio
	double W; ///< Viewing plane width (in m)
	double H; ///< Viewing plane height (in m)
	double pX; ///< Width of a pixel in screen-space (in m)
	double pY; ///< Height of a pixel in screen-space (in m)

	int screenWidthPixels; ///< Width of the viewing window (in pixels)
	int screenHeightPixels; ///< Height of the viewing window (in pixels)
	
	plane vPlane; ///< The viewing plane of the camera
	
	bool drawNorm; ///< Draw normal vectors on each polygon
	
	vector3 uX; ///< Unit vector for the x-axis
	vector3 uY; ///< Unit vector for the y-axis
	vector3 uZ; ///< Unit vector for the z-axis
	
	vector3 light; ///< Global light direction

	sdlWindow *window; ///< Pointer to the main renderer window

	/** Initialize the camera by setting initial values, computing all geometric parameters
	  * and opening the SDL window.
	  */
	void initialize();
	
	/** Convert a real-space position on the viewing plane to a position in screen-space [-1, 1]
	  * @note The origin of screen-space is the center of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the top
	  * @param vec The position on the surface of the viewing plane (in real-space)
	  * @param x The horizontal component of the screen-space coordinate
	  * @param y The vertical component of the screen-space coordinate
	  */
	void convertToScreenSpace(const vector3 &vec, double &x, double &y);
	
	/** Convert screen-space coordinates [-1, 1] to pixel-space
	  * @note The origin of pixel-space is the upper-left corner of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the bottom
	  * @param x The horizontal component of the screen-space coordinate
	  * @param y The vertical component of the screen-space coordinate
	  * @param px The horizontal pixel corresponding to the input x-coordinate
	  * @param py The vertical pixel corresponding to the input y-coordinate
	  */
	void convertToPixelSpace(const double &x, const double &y, int &px, int &py);

	/** Compute the coordinates where a ray intersects the viewing plane
	  * @param vertex The originating point of the ray (in real-space)
	  * @param sX The horizontal component of the position where the ray intersects the viewing plane (in screen-space)
	  * @param sY The vertical component of the position where the ray intersects the viewing plane (in screen-space)
	  * @return True if the ray intersects the viewing plane, and reutrn false otherwise
	  */
	bool compute(const vector3 &vertex, double &sX, double &sY);
	
	/** Trace a ray, from the camera position, through a point on the viewing plane to the surface-plane of an input triangle
	  * @param sX The x-coordinate on the viewing plane through which the ray will be cast (in screen-space)
	  * @param sY The y-coordinate on the viewing plane through which the ray will be cast (in screen-space)
	  * @param tri The triangle whose surface-plane will be intersected by the ray
	  * @param P The point (in real-space) where the ray intersects the surface-plane of the triangle
	  * @return True if the ray intersects with the surface-plane of the triangle, and return false otherwise
	  */
	bool rayTrace(const double &sX, const double &sY, const triangle &tri, vector3 &P);
	
	/** Draw the outline of a triangle to the screen
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param color The line color of the triangle
	  * @note There must be AT LEAST three elements in each array
	  */
	void drawTriangle(const int *x, const int *y, const sdlColor &color);
	
	/** Draw a filled triangle to the screen
	  * @param x Array of X pixel coordinates
	  * @param y Array of Y pixel coordinates
	  * @param color The fill color of the triangle
	  * @note There must be AT LEAST three elements in each array
	  */
	void drawFilledTriangle(const int *x, const int *y, const sdlColor &color);
};

#endif
