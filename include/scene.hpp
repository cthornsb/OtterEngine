#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <cstddef>
#include <memory>

#include "OTTFrameTimer.hpp"
#include "Graphics.hpp"
#include "lightSource.hpp"
#include "PixelTriplet.hpp"
#include "Vector.hpp"

class object;
class camera;
class triangle;

#ifndef SOFTWARE_RENDERER
class OTTWindow3D;
#else
class OTTWindow;
#endif // ifndef SOFTWARE_RENDERER

/** Supported drawing modes
  */
enum class drawMode {
	WIREFRAME, ///< Draw outlines of triangles with no backface culling
	MESH,      ///< Draw outlines of triangles with backface culling
	SOLID,     ///< Draw filled triangles with wireframe outline overlayed
	RENDER     ///< Draw filled triangles with light shading
};

/** @class scene
  * @brief 
  * @author Cory R. Thornsberry
  * @date September 5, 2019
  */

class scene : public OTTFrameTimer {
public:
	/** Default constructor
	  */
	scene();
	
	/** Constructor taking a pointer to a camera
	  */
	scene(camera *cam_);

	/** Destructor
	  */
	~scene();

	/** Opening the graphics window
	  */
	void initialize();

	/** Return true if the window is still open, and return false otherwise
	  */
	bool getStatus() const {
		return isRunning;
	}

#ifdef SOFTWARE_RENDERER
	/** Get the width of the screen (in pixels)
	  */
	int getScreenWidth() const {
		return screenWidthPixels;
	}

	/** Get the height of the screen (in pixels)
	  */	
	int getScreenHeight() const {
		return screenHeightPixels;
	}
#endif // ifdef SOFTWARE_RENDERER

	/** Get a pointer to the main camera
	  */
	camera *getCamera() {
		return cam;
	}

	/* Get a pointer to the main rendering window */
#ifndef SOFTWARE_RENDERER
	OTTWindow3D* getWindow() {
		return window.get();
	}
#else
	OTTWindow* getWindow() {
		return window.get();
	}
#endif // ifndef SOFTWARE_RENDERER

	/** Get a pointer to the world light source
	  */
	lightSource *getWorldLight() {
		return &worldLight;
	}

	/** Get a pointer to the last user keypress event
	  */
	OTTKeyboard* getKeypress();
	
	/** Get a pointer to the last user mouse event
	  */
	OTTMouse* getMouse();

#ifdef SOFTWARE_RENDERER
	/** Get the drawing mode to use when drawing the object to the screen
	  */
	drawMode getDrawingMode() const {
		return mode;
	}

	/** Set the width of the screen (in pixels)
	  */
	void setScreenWidth(const int &width) {
		screenWidthPixels = width;
	}
	
	/** Set the height of the screen (in pixels)
	  */
	void setScreenHeight(const int &height) {
		screenHeightPixels = height;
	}

	/** Set the drawing mode to use when drawing the object to the screen
	  */
	void setDrawingMode(const drawMode& dmode) {
		mode = dmode;
	}	
#endif // ifdef SOFTWARE_RENDERER

	/** Set the main camera for rendering and update its aspect ratio
	  */
	void setCamera(camera *cam_);

	/** Enable or disable the drawing of triangle normals
	  */
	void setDrawNormals(const bool &enable=true) {
		drawNorm = enable;
	}

	/** Enable or disable the drawing of the X, Y, and Z axes at the origin
	  */
	void setDrawOrigin(const bool &enable=true) {
		drawOrigin = enable;
	}

	/** Enable or disable shading objects with their z-depth value
	  */
	void setDrawZDepth(const bool& enable = true) {
		drawDepthMap = enable;
	}

	/** Enable vertical sync (VSync), assuming the platform and hardware allow it
	  */
	void enableVSync();

	/** Disable vertical sync (VSync) and set the new target framerate 
	  */
	void disableVSync(const double& fps=60.0);

	/** Add an object to the list of objects to be rendered
	  */
	void addObject(object* obj);
	
	/** Add a light to the list of lights to be rendered
	  */
	void addLight(lightSource* light) {
		lights.push_back(light);
	}

	/** Clear the screen by filling it with a color (black by default)
	  */
	void clear(const ColorRGB &color=Colors::BLACK);
	
	/** Draw objects on the screen using OpenGL renderer (or software renderer)
	  * @note This method should be called once per iteration of the main loop
	  * @return True if the update was successful and return false if the user closed the window
	  */
	bool update();
	
private:
	bool drawNorm; ///< Flag indicating that normal vectors will be drawn on each triangle

	bool drawOrigin; ///< Flag indicating that the X, Y, and Z axes will be drawn at the origin

	bool drawDepthMap; ///< "Shade" objects with their Z-depth (i.e. their depth into the screen)

	bool isRunning; ///< Flag indicating that the window is still open and active

	camera *cam; ///< Pointer to the main camera

#ifndef SOFTWARE_RENDERER
	std::unique_ptr<OTTWindow3D> window; ///< Pointer to the main renderer window
#else
	int screenWidthPixels; ///< Width of the viewing window (in pixels)

	int screenHeightPixels; ///< Height of the viewing window (in pixels)

	int minPixelsX;

	int minPixelsY;
	
	int maxPixelsX;

	int maxPixelsY;

	drawMode mode; ///< Current rendering mode

	std::unique_ptr<OTTWindow> window; ///< Pointer to the main renderer window
#endif // ifndef SOFTWARE_RENDERER

	directionalLight worldLight; ///< Global light source
	
	std::vector<object*> objects; ///< Vector of pointers to all 3d objects currently in the scene
	
	std::vector<lightSource*> lights; ///< Vector of pointers to all light sources currently in the scene

#ifdef SOFTWARE_RENDERER
	std::vector<pixelTriplet> polygonsToDraw; ///< Vector of all polygons to draw (software renderer)
#endif // ifdef SOFTWARE_RENDERER

	/** Refresh the screen
	  * @return True if the update was successful and return false if the user closed the window
	  */
	bool render();

#ifdef SOFTWARE_RENDERER
	/** Process all polygons of an object
	  */
	void processPolygons(object* obj);

	/** Process an object and all its child objects
	  */
	void processObject(object *obj);

	/**
	  */
	void convertToScreenSpace(const int& px, const int& py, float& x, float& y) const;

	/**
	  */
	bool checkScreenSpace(const float& x, const float& y) const;

	/** Convert screen-space coordinates [-1, 1] to pixel-space
	  * @note The origin of pixel-space is the upper-left corner of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the bottom
	  * @param x The horizontal component of the screen-space coordinate
	  * @param y The vertical component of the screen-space coordinate
	  * @param px The horizontal pixel corresponding to the input x-coordinate
	  * @param py The vertical pixel corresponding to the input y-coordinate
	  * @return True if the point is on the screen and return false otherwise
	  */
	bool convertToPixelSpace(const float& x, const float& y, int &px, int &py) const;

	/** Convert screen-space coordinates [-1, 1] to pixel-space
	  * @note The origin of pixel-space is the upper-left corner of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the bottom
	  * @param coords The pixel coordinate holder for the three vertex projections
	  * @return True if at least one of the vertices is on the screen and return false otherwise
	  */
	bool convertToPixelSpace(pixelTriplet &coords) const;
#endif // ifdef SOFTWARE_RENDERER

	/** Draw a point to the screen
	  * @param point The point in 3d space to draw
	  * @param color The color of the point
	  */
	void drawPoint(const Vector3 &point, const ColorRGB &color);

	/** Draw a vector to the screen
	  * @param start The start point of the vector to draw
	  * @param direction The direction of the vector to draw
	  * @param color The color of the vector
	  * @param length The total length to draw
	  */	
	void drawVector(const Vector3 &start, const Vector3 &direction, const ColorRGB &color, const float &length=1);
	
	/** Draw a ray to the screen
	  * @param proj The 3d ray to draw
	  * @param color The color of the ray
	  * @param length The total length to draw
	  */
	void drawRay(const ray &proj, const ColorRGB &color, const float &length=1);

#ifdef SOFTWARE_RENDERER
	/** Draw the outline of a triangle to the screen
	  * @param coords The pixel coordinate holder for the three vertex projections
	  * @param color The line color of the triangle
	  * @note There must be AT LEAST three elements in each array
	  */
	void drawTriangle(const pixelTriplet &coords, const ColorRGB &color);
	
	/** Draw a filled triangle to the screen
	  * @param coords The pixel coordinate holder for the three vertex projections
	  * @param color The fill color of the triangle
	  * @note There must be AT LEAST three elements in each array
	  */
	void drawFilledTriangle(const pixelTriplet &coords, const ColorRGB &color);
#endif // ifdef SOFTWARE_RENDERER

	/** Draw X, Y, and Z unit vector axes
	  */
	void drawAxes();
};

#endif
