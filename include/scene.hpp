#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <cstddef>

#include "lightSource.hpp"

class sdlWindow;
class object;
class camera;
class lightSource;

class scene{
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
	scene();
	
	/** Constructor taking a pointer to a camera
	  */
	scene(camera *cam_);

	/** Destructor
	  */
	~scene();

	/** Opening the SDL window
	  */
	void initialize();

	/** Return true if the window is still open, and return false otherwise
	  */
	bool getStatus() const { return isRunning; }

	/** Get the width of the screen (in pixels)
	  */
	int getScreenWidth() const { return screenWidthPixels; }

	/** Get the height of the screen (in pixels)
	  */	
	int getScreenHeight() const { return screenHeightPixels; }
	
	/** Get a pointer to the main camera
	  */
	camera *getCamera(){ return cam; }

	/** Get a pointer to the world light source
	  */
	lightSource *getWorldLight(){ return &worldLight; }

	/** Set the width of the screen (in pixels)
	  */
	void setScreenWidth(const int &width){ screenWidthPixels = width; }
	
	/** Set the height of the screen (in pixels)
	  */
	void setScreenHeight(const int &height){ screenHeightPixels = height; }
	
	/** Set the main camera for rendering
	  */
	void setCamera(camera *cam_){ cam = cam_; }

	/** Enable or disable the drawing of triangle normals
	  */
	void setDrawNormals(const bool &enable=true){ drawNorm = enable; }

	/** Add an object to the list of objects to be rendered
	  */
	void addObject(object *obj){ objects.push_back(obj); }
	
	/** Add a light to the list of lights to be rendered
	  */
	void addLight(const lightSource &light){ lights.push_back(light); }

	/** Render a 3d object
	  * @param obj Pointer to the object to draw
	  */
	void render(object* obj);

	/** Clear the screen by filling it with a color (black by default)
	  */
	void clear(const sdlColor &color=Colors::BLACK);
	
	/** Update the screen
	  * @note This method will check whether or not the user has closed the window. If so,
	  *       this method will set the @a isRunning flag to false and return
	  */
	void update();

	/** Wait until the user closes the screen, and then return
	  */
	void loop();

private:
	double timeElapsed;

	bool drawNorm; ///< Flag indicating that normal vectors will be drawn on each triangle
	bool isRunning; ///< Flag indicating that the window is still open and active

	int screenWidthPixels; ///< Width of the viewing window (in pixels)
	int screenHeightPixels; ///< Height of the viewing window (in pixels)

	camera *cam;
	
	sdlWindow *window; ///< Pointer to the main renderer window
	
	directionalLight worldLight; ///< Global light source
	
	std::vector<object*> objects;
	
	std::vector<lightSource> lights;

	/** 
	  */
	void processObject(object *obj);

	/** Convert screen-space coordinates [-1, 1] to pixel-space
	  * @note The origin of pixel-space is the upper-left corner of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the bottom
	  * @param x The horizontal component of the screen-space coordinate
	  * @param y The vertical component of the screen-space coordinate
	  * @param px The horizontal pixel corresponding to the input x-coordinate
	  * @param py The vertical pixel corresponding to the input y-coordinate
	  */
	void convertToPixelSpace(const double &x, const double &y, int &px, int &py);

	/** Draw a vector to the screen
	  * @param start The start point of the vector to draw
	  * @param direction The direction of the vector to draw
	  * @param proj The total length to draw
	  */	
	void drawVector(const vector3 &start, const vector3 &direction, const sdlColor &color, const double &length=1);
	
	/** Draw a ray to the screen
	  * @param proj The 3d ray to draw (in real-space)
	  * @param proj The total length to draw
	  */
	void drawRay(const ray &proj, const sdlColor &color, const double &length=1);
	
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
