#ifndef GRAPHICS_3D_OTT_WINDOW_3D_HPP
#define GRAPHICS_3D_OTT_WINDOW_3D_HPP

#include "OTTShader3D.hpp"

#include <graphics/core/OTTWindow.hpp>
#include <math/OTTVector.hpp>

#include <vector>
#include <string>

namespace ott {

class Object;
class Texture;

/// <summary>
/// Wrapper class for interfacing with glfw and OpenGL, and for managing graphical windows
/// </summary>
class Window3d : public Window {
public:
	/** Default constructor
	  */
	Window3d() = delete;
	
	/** Constructor taking the width and height of the window
	  */
	Window3d(const int32_t &w, const int32_t &h, const int32_t& scale=1) :
		Window(w, h),
		b3dMode(true),
		fNearPlane(0.1f),
		fFarPlane(100.f),
		fFieldOfView(90.f),
		shaders()
	{
	}

	/** Destructor
	  */
	~Window3d();
	
	/** Get a pointer to a loaded shader
	  */
	Shader3d* GetShader(const ShaderType& type);

	/** Draw a point at position (x, y, z) in the scene
	  */
	void DrawPixel(const float& x, const float& y, const float& z);

	/** Draw a 3d line between points (x1, y1, z1) and (x2, y2, z2) in the scene
	  */
	void DrawLine(
		const float& x1, const float& y1, const float& z1,
		const float& x2, const float& y2, const float& z2
	);

	/** Draw a 3d line between points p1=(x1, y1, z1) and p2=(x2, y2, z2) in the scene
	  */
	void DrawLine(const Vector3& p1, const Vector3& p2);

	/** Draw a 3d polyline in the scene
	  * @param points Vector of polyline vertices
	  */
	void DrawPolyline(const std::vector<Vector3>& points);

	/** Draw a closed 3d polygon in the scene
	  * @param points Vector of polygon vertices
	  */
	void DrawPolygon(const std::vector<Vector3>& points);

	/** Draw a 2d texture on the screen within the specified pixel bounds.
	  * Does not allow for 2d rotation.
	  * @param texture Pointer to OpenGL texture
	  * @param p1 Top left vertex of texture (in screen space)
	  * @param p2 Bottom right vertex of texture (in screen space)
	  * @param depth Depth into the scene (as a fraction of the distance between the near and far clipping planes)
	 **/
	void DrawTexture(const GLuint& texture, const Vector2& p1, const Vector2& p2, const float& depth = 0.f);

	/** Draw a 2d texture on the screen within the specified NDC bounds (-1 to 1).
	  * Does not allow for 2d rotation.
	  * @param texture Pointer to OpenGL texture
	  * @param p1 Bottom left vertex of texture (in NDC space)
	  * @param p2 Top right vertex of texture (in NDC space)
	  * @param depth Depth into the scene (in NDC space)
	 **/
	void DrawTextureNDC(const GLuint& texture, const Vector2& p1, const Vector2& p2, const float& depth = -1.f);

	/** Draw an array of raw 3d vertices
	  * @param vertices Array of raw 3d position components formatted as {x0, y0, z0, ... , xN-1, yN-1, zN-1}
	  * @param indicies Vector of vertex indicies representing the triangles which will be drawn
	  */
	void DrawVertexArray(const float* vertices, const std::vector<uint16_t>& indices);

	/** Draw a 3d object on the screen
	  * @param obj Pointer to 3d object which will be drawn
	  */
	void DrawObject(const Object* obj);

	/** Draw a static 3d object on the screen
	  * @param obj Pointer to 3d object which will be drawn
	  * @param offset Position offset of object (camera position)
	  */
	void DrawStaticObject(const Object* obj, const Vector3& offset);

	/** Draw X, Y, and Z unit vector axes
	  */
	void DrawAxes();

	/** Enable OpenGL Z-depth test and setup viewport for perspective 3d mode
	  * This method must be called for rendering 3d objects as the default rendering mode is 2d
	  **/
	void EnableZDepth();

	/** Disable OpenGL Z-depth test and setup viewport for 2d drawing mode (default)
	  **/
	void DisableZDepth();

	/** Enable OpenGL backface culling
	  * OpenGL perspective mode must be enabled, otherwise this will have no effect
	  **/
	void EnableCulling();

	/** Disable OpenGL backface culling
	  */
	void DisableCulling();

	/** Switch to wireframe mode
	  * OpenGL perspective mode must be enabled, otherwise this will have no effect
	  */
	void EnableWireframeMode();

	/** Disable wireframe mode (default)
	  */
	void DisableWireframeMode();

	/** Enable the use of an OpenGL shader program
	  */
	void EnableShader(const ShaderType& type);

	/** Disable the use of an OpenGL shader program
	  */
	void DisableShader();

	/** Reset the OpenGL modelview matrix to identity
	  */
	void ResetModelviewMatrix();

	/** Translate OpenGL modelview matrix by a translation vector
	  */
	void TranslateModelviewMatrix(const Vector3& pos);

	/** Rotate OpenGL modelview matrix by angles about the x, y, and z axes (in radians)
	  */
	void RotateModelviewMatrix(const float& x, const float& y, const float& z);

	/** Handle window resizes for 3d scenes
	  * @param width The new width of the window after the user has resized it
	  * @param height The new height of the window after the user has resized it
	  */
	static void HandleReshapeScene3D(GLFWwindow* window, int32_t width, int32_t height);

protected:
	bool b3dMode; ///< Set if a 3d projection matrix is being used

	float fNearPlane;
	
	float fFarPlane;
	
	float fFieldOfView;

	std::unique_ptr<shaders::ShaderList> shaders;

	/** Update viewport and 3d projection matrix after resizing window
	  */
	void Reshape3D();
	
	/** Called whenever the user updates the size of the window
	  */
	void OnUserReshape() override;
	
	/** Initialize GLEW (if this is the first initialization) and create a new shader list
	  */
	void OnUserInitialize() override;
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_3D_OTT_WINDOW_3D_HPP
