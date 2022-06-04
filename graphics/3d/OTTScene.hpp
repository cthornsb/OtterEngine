#ifndef GRAPHICS_3D_OTT_SCENE_HPP
#define GRAPHICS_3D_OTT_SCENE_HPP

#include <core/OTTFrameTimer.hpp>
#include <graphics/3d/OTTLightSource.hpp>
#include <graphics/core/OTTColor.hpp>

#ifndef SOFTWARE_RENDERER
#include <graphics/3d/OTTWindow3d.hpp>
#else
#include <graphics/core/OTTWindow.hpp>
#endif // ifndef SOFTWARE_RENDERER

#include <vector>
#include <memory>

namespace ott {

/** Supported drawing modes
  */
enum class DrawMode {
	Wireframe, ///< Draw outlines of triangles with no backface culling
	Mesh,      ///< Draw outlines of triangles with backface culling
	Solid,     ///< Draw filled triangles with wireframe outline overlayed
	Render     ///< Draw filled triangles with light shading
};

class Camera;
class Keyboard;
class LightSource;
class Mouse;
class Object;

/** @class scene
  * @brief 
  * @author Cory R. Thornsberry
  * @date September 5, 2019
  */

class Scene : public FrameTimer {
public:
	/** Default constructor
	  */
	Scene();
	
	/** Constructor taking a pointer to a camera
	  */
	Scene(Camera *cam_);

	/** Destructor
	  */
	~Scene();

	/** Opening the graphics window
	  */
	void Initialize();

	/** Return true if the window is still open, and return false otherwise
	  */
	bool Status() const {
		return isRunning;
	}

#ifdef SOFTWARE_RENDERER
	/** Get the width of the screen (in pixels)
	  */
	int32_t ScreenWidth() const {
		return screenWidthPixels;
	}

	/** Get the height of the screen (in pixels)
	  */	
	int32_t ScreenHeight() const {
		return screenHeightPixels;
	}
#endif // ifdef SOFTWARE_RENDERER

	/** Get a pointer to the main camera
	  */
	Camera* GetCamera() {
		return cam;
	}

	/* Get a pointer to the main rendering window */
#ifndef SOFTWARE_RENDERER
	Window3d* GetWindow() {
		return window.get();
	}
#else
	OTTWindow* GetWindow() {
		return window.get();
	}
#endif // ifndef SOFTWARE_RENDERER

	/** Get a pointer to the world light source
	  */
	LightSource *GetWorldLight() {
		return &worldLight;
	}

	/** Get a pointer to the last user keypress event
	  */
	Keyboard* GetKeyboard();
	
	/** Get a pointer to the last user mouse event
	  */
	Mouse* GetMouse();

#ifdef SOFTWARE_RENDERER
	/** Get the drawing mode to use when drawing the object to the screen
	  */
	DrawMode GetDrawingMode() const {
		return mode;
	}

	/** Set the width of the screen (in pixels)
	  */
	void SetScreenWidth(const int32_t &width) {
		screenWidthPixels = width;
	}
	
	/** Set the height of the screen (in pixels)
	  */
	void SetScreenHeight(const int32_t &height) {
		screenHeightPixels = height;
	}

	/** Set the drawing mode to use when drawing the object to the screen
	  */
	void SetDrawingMode(const DrawMode& dmode) {
		mode = dmode;
	}	
#endif // ifdef SOFTWARE_RENDERER

	/** Set the main camera for rendering and update its aspect ratio
	  */
	void SetCamera(Camera *cam_);

	/** Enable or disable the drawing of the X, Y, and Z axes at the origin
	  */
	void SetDrawOrigin(const bool &enable=true) {
		drawOrigin = enable;
	}

	/** Enable or disable shading objects with their z-depth value
	  */
	void SetDrawZDepth(const bool& enable = true) {
		drawDepthMap = enable;
	}

	/** Enable vertical sync (VSync), assuming the platform and hardware allow it
	  */
	void EnableVSync();

	/** Disable vertical sync (VSync) and set the new target framerate 
	  */
	void DisableVSync(const double& fps=60.0);

	/** Add an object to the list of objects to be rendered
	  */
	void AddObject(Object* obj);
	
	/** Add a light to the list of lights to be rendered
	  */
	void AddLight(LightSource* light) {
		lights.push_back(light);
	}

	/** Clear the screen by filling it with a color (black by default)
	  */
	void Clear(const ColorRGB &color=colors::Black);
	
	/** Draw objects on the screen using OpenGL renderer (or software renderer)
	  * @return True if the update was successful and return false if the user closed the window
	  */
	bool DrawOpenGL();

private:
	bool drawOrigin; ///< Flag indicating that the X, Y, and Z axes will be drawn at the origin

	bool drawDepthMap; ///< "Shade" objects with their Z-depth (i.e. their depth into the screen)

	bool isRunning; ///< Flag indicating that the window is still open and active

	Camera* cam; ///< Pointer to the main camera

#ifndef SOFTWARE_RENDERER
	std::unique_ptr<Window3d> window; ///< Pointer to the main renderer window
#else
	int32_t screenWidthPixels; ///< Width of the viewing window (in pixels)

	int32_t screenHeightPixels; ///< Height of the viewing window (in pixels)

	int32_t minPixelsX;

	int32_t minPixelsY;
	
	int32_t maxPixelsX;

	int32_t maxPixelsY;

	drawMode mode; ///< Current rendering mode

	std::unique_ptr<OTTWindow> window; ///< Pointer to the main renderer window
#endif // ifndef SOFTWARE_RENDERER

	DirectionalLight worldLight; ///< Global light source
	
	std::vector<Object*> objects; ///< Vector of pointers to all 3d objects currently in the scene
	
	std::vector<LightSource*> lights; ///< Vector of pointers to all light sources currently in the scene

#ifdef SOFTWARE_RENDERER
	std::vector<PixelTriplet> polygonsToDraw; ///< Vector of all polygons to draw (software renderer)
#endif // ifdef SOFTWARE_RENDERER

	/** Refresh the screen
	  * @return True if the update was successful and return false if the user closed the window
	  */
	bool Render();

#ifdef SOFTWARE_RENDERER
	/** Process all polygons of an object
	  */
	void ProcessPolygons(Object* obj);

	/** Process an object and all its child objects
	  */
	void ProcessObject(Object *obj);

	/**
	  */
	void ConvertToScreenSpace(const int32_t& px, const int32_t& py, float& x, float& y) const;

	/**
	  */
	bool CheckScreenSpace(const float& x, const float& y) const;

	/** Convert screen-space coordinates [-1, 1] to pixel-space
	  * @note The origin of pixel-space is the upper-left corner of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the bottom
	  * @param x The horizontal component of the screen-space coordinate
	  * @param y The vertical component of the screen-space coordinate
	  * @param px The horizontal pixel corresponding to the input x-coordinate
	  * @param py The vertical pixel corresponding to the input y-coordinate
	  * @return True if the point is on the screen and return false otherwise
	  */
	bool ConvertToPixelSpace(const float& x, const float& y, int32_t &px, int32_t &py) const;

	/** Convert screen-space coordinates [-1, 1] to pixel-space
	  * @note The origin of pixel-space is the upper-left corner of the screen with the positive x-direction
	  *       being toward the right side of the screen and the positive y-direction being toward the bottom
	  * @param coords The pixel coordinate holder for the three vertex projections
	  * @return True if at least one of the vertices is on the screen and return false otherwise
	  */
	bool ConvertToPixelSpace(PixelTriplet &coords) const;
#endif // ifdef SOFTWARE_RENDERER

	/** Draw a point to the screen
	  * @param point The point in 3d space to draw
	  * @param color The color of the point
	  */
	void DrawPoint(const Vector3 &point, const ColorRGB &color);

	/** Draw a vector to the screen
	  * @param start The start point of the vector to draw
	  * @param direction The direction of the vector to draw
	  * @param color The color of the vector
	  * @param length The total length to draw
	  */	
	void DrawVector(const Vector3 &start, const Vector3 &direction, const ColorRGB &color, const float &length=1);
	
	/** Draw a ray to the screen
	  * @param proj The 3d ray to draw
	  * @param color The color of the ray
	  * @param length The total length to draw
	  */
	void DrawRay(const Ray_t &proj, const ColorRGB &color, const float &length=1);

#ifdef SOFTWARE_RENDERER
	/** Draw the outline of a triangle to the screen
	  * @param coords The pixel coordinate holder for the three vertex projections
	  * @param color The line color of the triangle
	  * @note There must be AT LEAST three elements in each array
	  */
	void DrawTriangle(const PixelTriplet &coords, const ColorRGB &color);
	
	/** Draw a filled triangle to the screen
	  * @param coords The pixel coordinate holder for the three vertex projections
	  * @param color The fill color of the triangle
	  * @note There must be AT LEAST three elements in each array
	  */
	void DrawFilledTriangle(const PixelTriplet &coords, const ColorRGB &color);
#endif // ifdef SOFTWARE_RENDERER
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_3D_OTT_SCENE_HPP
