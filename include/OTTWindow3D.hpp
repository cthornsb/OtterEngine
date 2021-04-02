#ifndef OTT_WINDOW_3D_HPP
#define OTT_WINDOW_3D_HPP

#include <vector>
#include <string>

#include "OTTWindow.hpp"
#include "OTTShader.hpp"
#include "Vector.hpp"

class object;

/// <summary>
/// Wrapper class for interfacing with glfw and OpenGL, and for managing graphical windows
/// </summary>
class OTTWindow3D : public OTTWindow {
public:
	/** Default constructor
	  */
	OTTWindow3D() = delete;
	
	/** Constructor taking the width and height of the window
	  */
	OTTWindow3D(const int &w, const int &h, const int& scale=1) : 
		OTTWindow(w, h),
		b3dMode(true),
		fNearPlane(0.1f),
		fFarPlane(100.f),
		fFieldOfView(90.f),
		shaders()
	{
	}

	/** Destructor
	  */
	~OTTWindow3D();
	
	/** Get a pointer to a loaded shader
	  */
	OTTShader* getShader(const ShaderType& type);

	/** Draw a point at position (x, y, z) in the scene
	  */
	void drawPixel(const float& x, const float& y, const float& z);

	/** Draw a 3d line between points (x1, y1, z1) and (x2, y2, z2) in the scene
	  */
	void drawLine(
		const float& x1, const float& y1, const float& z1,
		const float& x2, const float& y2, const float& z2
	);

	/** Draw a 3d line between points p1=(x1, y1, z1) and p2=(x2, y2, z2) in the scene
	  */
	void drawLine(const Vector3& p1, const Vector3& p2);

	/** Draw a 3d polyline in the scene
	  * @param points Vector of polyline vertices
	  */
	void drawPolyline(const std::vector<Vector3>& points);

	/** Draw a closed 3d polygon in the scene
	  * @param points Vector of polygon vertices
	  */
	void drawPolygon(const std::vector<Vector3>& points);

	/** Draw OpenGL texture in the scene within the specified bounds
	  * @param texture GL texture context
	  * @param p1 Top left vertex of texture
	  * @param p2 Bottom right vertex of texture
	  * @param norm Normal vector of texture face
	 **/
	void drawTexture(const unsigned int& texture, const Vector3& p1, const Vector3& p2, const Vector3& norm);

	/** Draw an array of raw 3d vertices
	  * @param vertices Array of raw 3d position components formatted as {x0, y0, z0, ... , xN-1, yN-1, zN-1}
	  * @param indicies Vector of vertex indicies representing the triangles which will be drawn
	  */
	void drawVertexArray(const float* vertices, const std::vector<unsigned short>& indices);

	/** Draw a 3d object on the screen
	  * @param obj Pointer to 3d object which will be drawn
	  */
	void drawObject(const object* obj);

	/** Draw a static 3d object on the screen
	  * @param obj Pointer to 3d object which will be drawn
	  * @param offset Position offset of object (camera position)
	  */
	void drawStaticObject(const object* obj, const Vector3& offset);

	/** Draw X, Y, and Z unit vector axes
	  */
	void drawAxes();

	/** Enable OpenGL alpha blending for translucent textures
	  **/
	void enableAlphaBlending();

	/** Enable OpenGL Z-depth test and setup viewport for perspective 3d mode
	  * This method must be called for rendering 3d objects as the default rendering mode is 2d
	  **/
	void enableZDepth();

	/** Disable OpenGL Z-depth test and setup viewport for 2d drawing mode (default)
	  **/
	void disableZDepth();

	/** Enable OpenGL backface culling
	  * OpenGL perspective mode must be enabled, otherwise this will have no effect
	  **/
	void enableCulling();

	/** Disable OpenGL backface culling
	  */
	void disableCulling();

	/** Switch to wireframe mode
	  * OpenGL perspective mode must be enabled, otherwise this will have no effect
	  */
	void enableWireframeMode();

	/** Disable wireframe mode (default)
	  */
	void disableWireframeMode();

	/** Enable the use of an OpenGL shader program
	  */
	void enableShader(const ShaderType& type);

	/** Disable the use of an OpenGL shader program
	  */
	void disableShader();

	/** Reset the OpenGL modelview matrix to identity
	  */
	void resetModelviewMatrix();

	/** Translate OpenGL modelview matrix by a translation vector
	  */
	void translateModelviewMatrix(const Vector3& pos);

	/** Rotate OpenGL modelview matrix by angles about the x, y, and z axes (in radians)
	  */
	void rotateModelviewMatrix(const float& x, const float& y, const float& z);

	/** Handle window resizes for 3d scenes
	  * @param width The new width of the window after the user has resized it
	  * @param height The new height of the window after the user has resized it
	  */
	static void handleReshapeScene3D(GLFWwindow* window, int width, int height);

protected:
	bool b3dMode; ///< Set if a 3d projection matrix is being used

	float fNearPlane;
	
	float fFarPlane;
	
	float fFieldOfView;

	std::unique_ptr<OTTDefaultShaders::ShaderList> shaders;

	/** Update viewport and 3d projection matrix after resizing window
	  */
	void reshape3D();
	
	/** Called whenever the user updates the size of the window
	  */
	void onUserReshape() override;
	
	/** Initialize GLEW (if this is the first initialization) and create a new shader list
	  */
	void onUserInitialize() override;
};
#endif
