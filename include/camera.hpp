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

class camera : public ray {
public:
	enum drawMode {WIREFRAME, MESH, SOLID, RENDER};

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
	  * @param dist The distance to move
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

	void clear();
	
	void render();

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

	void initialize();
	
	void convertToScreenSpace(const vector3 &vec, double &x, double &y);
	
	void convertToPixelSpace(const double &x, const double &y, int &px, int &py);

	bool compute(const vector3 &vertex, double &sX, double &sY);
	
	bool rayTrace(const double &sX, const double &sY, const triangle &tri, vector3 &P);
};

#endif
