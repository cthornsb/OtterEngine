#ifndef GRAPHICS_GEOMETRY_OTT_OBJECT_HPP
#define GRAPHICS_GEOMETRY_OTT_OBJECT_HPP

#include "OTTPolygon.hpp"
#include "OTTPolygonContainer.hpp"
#include "OTTVertexContainer.hpp"

#include <graphics/core/OTTColor.hpp>
#include <math/OTTMatrix.hpp>
#include <math/OTTWrappedValue.hpp>

#include <vector>

namespace ott {

class Camera;
class Shader3d;
class Texture;
class Window3d;

/// <summary>
/// 3d object geometry class
/// </summary>
class Object{
public:
	/** Default constructor
	  */
	Object();

	/** Object position constructor
	  */	
	Object(const Vector3& pos_);
	
	/** Get a pointer to the vector of polygons which comprise this 3d object
	  */
	std::vector<Polygon>* PolygonVector() {
		return polys.Polygons();
	}

	/** Get the position offset of the object
	  */
	Vector3 Position() const {
		return position;
	}

	/** Get the position of the center of the object
	  */
	Vector3 Center() const {
		return (center + position);
	}

	/** Get the size of the object along the x-axis
	  */
	float SizeX() const {
		return (maxSize[0] - minSize[0]);
	}

	/** Get the size of the object along the y-axis
	  */
	float SizeY() const {
		return (maxSize[1] - minSize[1]);
	}

	/** Get the size of the object along the z-axis
	  */
	float SizeZ() const {
		return (maxSize[2] - minSize[2]);
	}

	/** Get the current pitch angle (angle about the x-axis) (in radians)
	  */
	float PitchAngle() const {
		return pitchAngle.Get();
	}

	/** Get the current roll angle (angle about the z-axis) (in radians)
	  */
	float RollAngle() const {
		return rollAngle.Get();
	}

	/** Get the current yaw angle (angle about the y-axis) (in radians)
	  */
	float YawAngle() const {
		return yawAngle.Get();
	}

	/** Get a const pointer to the position vector
	  */
	const Vector3* ConstPositionPointer() const {
		return (const Vector3*)&position;
	}

	/** Get a const pointer to the position vector
	  */
	Vector3* PositionPointer() {
		return &position;
	}

	/** Get a const pointer to the rotation matrix
	  */
	const Matrix3* ConstRotationMatrix() const {
		return (const Matrix3*)&rotation;
	}

	/** Get a pointer to the rotation matrix
	  */
	Matrix3* RotationMatrix() {
		return &rotation;
	}

	/** Get the number of unique vertices
	  */
	size_t NumberOfVertices() const {
		return vertices.size();
	}
	
	/** Get the number of unique polygons
	  */
	size_t NumberOfPolygons() const {
		return polys.Size();
	}

	/** Get the number of expected vertices
	  */
	size_t NumberOfReservedVertices() const {
		return reservedVertices;
	}

	/** Get the number of expected polygons
	  */
	size_t NumberOfReservedPolygons() const {
		return reservedPolygons;
	}

	/** Get a const pointer to this object's parent
	  * @return Pointer to this object's parent object, or (this) if object has no parent
	  */
	const Object* Parent() const {
		return (parent ? parent : this);
	}

	/** Get a const pointer to the shader used to render the object
	  */
	const Shader3d* GetShader() const {
		return shader;
	}

	/** Get a pointer to the vertex container object
	  */
	VertexContainer* Vertices() {
		return &vertices;
	}

	/** Get a pointer to the vertex container object
	  */
	const VertexContainer* ConstVertices() const {
		return &vertices;
	}

	/** Get a pointer to the polygon container object
	  */
	PolygonContainer* Polygons() {
		return &polys;
	}

	/** Get a pointer to the polygon container object
	  */
	const PolygonContainer* ConstPolygons() const {
		return &polys;
	}

	/** Get OpenGL VBO id number corresponding to object vertices
	  */
	uint32_t VertexVBO() const {
		return polys.VertexVBO();
	}

	/** Get OpenGL VBO id number corresponding to object vertex indicies
	  */
	uint32_t IndexVBO() const {
		return polys.IndexVBO();
	}

	/** Get the modelview matrix
	  */
	Matrix4* ModelMatrix();

	/** Get the modelview matrix, transformed by an external model matrix (e.g. the model matrix of a parent object)
	  */
	Matrix4* ModelMatrix(const Matrix4& mat);

	/** Get the object's local X-axis unit vector
	  */
	Vector3 UnitVectorX() const {
		return uX;
	}

	/** Get the object's local Y-axis unit vector
	  */
	Vector3 UnitVectorY() const {
		return uY;
	}

	/** Get the object's local Z-axis unit vector
	  */
	Vector3 UnitVectorZ() const {
		return uZ;
	}

	/** Get the current texture
	  */
	const Texture* GetTexture() const {
		return texture;
	}

	/** Retrun true if local object unit vectors should be drawn
	  */
	bool DrawOrigin() const {
		return bDrawOrigin;
	}

	/** Return true if face normals should be drawn
	  */
	bool DrawNormals() const {
		return bDrawNormals;
	}

	/** Return true if this object has a parent object and return false otherwise
	  */
	bool IsChild() const {
		return (parent != 0x0);
	}

	/** Return true if this object has child objects and return false otherwise
	  */
	bool HasChildren() const {
		return !children.empty();
	}

	/** Return true if this object is currently hidden (not rendered)
	  */
	bool IsHidden() const {
		return bHidden;
	}

	/** Get the object's ambient color
	  */
	ColorRGB AmbientColor() const {
		return ambientColor;
	}

	/** Get const iterator at the start of the child objects vector
	  */
	std::vector<Object*>::const_iterator BeginChildren() const {
		return children.cbegin();
	}

	/** Get const iterator at the end of the child objects vector
	  */
	std::vector<Object*>::const_iterator EndChildren() const {
		return children.cend();
	}

	/** Rotate the object by a given amount about the X, Y, and Z, axes (all in radians)
	  * @note This method will rotate vertices from their current position. Use setRotation() to specify the rotation explicitly
	  */
	void Rotate(const float& theta, const float& phi, const float& psi);

	/** Move the position of the object
	  * @note This method moves the object relative to its current position. Use setPosition() to specify the position explicitly
	  */
	void Move(const Vector3 &offset);

	/** Rotate the object to specified angles about the X, Y, and Z, axes (all in radians)
	  */
	void SetRotation(const float& theta, const float& phi, const float& psi);

	/** Set the position of the object
	  */
	void SetPosition(const Vector3 &pos);

	/** Set the shader to use for rendering. 
	  * Setting a new shader will automatically un-hide an object.
	  * @param shdr Pointer to the shader to use for rendering this object
	  * @param bSetChildShaders If set, specified shader will also be set for all child objects (if any)
	  */
	void SetShader(const Shader3d* shdr, bool bSetChildShaders=false);

	/** Scale object relative to its current size
	  */
	void SetScaleFactor(const float& scale_) {
		scaleFactor *= scale_;
	}

	/** Set scale of all three vector axes
	  */
	void SetScale(const float& scale_) {
		scaleFactor = Vector3(scale_, scale_, scale_);
	}

	/** Set scale of all three vector axes
	  */
	void SetScale(const Vector3& scale_) {
		scaleFactor = scale_;
	}

	/** Set scale of all three vector axes
	  */
	void SetScale(const float& xScale, const float& yScale, const float& zScale) {
		scaleFactor = Vector3(xScale, yScale, zScale);
	}

	/** Set scale of object's x-axis
	  */
	void SetScaleX(const float& scale_) {
		scaleFactor[0] = scale_;
	}

	/** Set scale of object's y-axis
	  */
	void SetScaleY(const float& scale_) {
		scaleFactor[1] = scale_;
	}

	/** Set scale of object's z-axis
	  */
	void SetScaleZ(const float& scale_) {
		scaleFactor[2] = scale_;
	}

	/** Set object's texture
	  * @param txt Texture to use for this object
	  * @param bSetChildShaders If set, specified shader will also be set for all child objects (if any)
	  */
	void SetTexture(Texture* txt, bool bSetChildren=false);

	/** Enable or disable drawing object's local unit vectors
	  */
	void SetDrawOrigin(bool state = true) {
		bDrawOrigin = state;
	}

	/** Enable or disable drawing object's face normals
	  */
	void SetDrawNormals(bool state = true) {
		bDrawNormals = state;
	}

	/** Hide the object from view
	  */
	void Hide() {
		bHidden = true;
	}

	/** Show the object (default)
	  */
	void Show() {
		bHidden = false;
	}

	/** Set the object's ambient color
	  */
	void SetAmbientColor(const ColorRGB& color) {
		ambientColor = color;
	}

	/** Reset the offset position of the object to its original location
	  */
	void ResetPosition();

	/** Project all vertices onto the view-plane
	  */
	void RenderAllVertices(Camera* cam);

	/** Add a child to this object.
	  * The child object's current world position is used as its offset within the parent object.
	  * @note An object with multiple parents may cause undefined behavior
	  * @param child Pointer to the child object
	  */
	void AddChild(Object* child);

	/** Remove a child from this object
	  * @param child Pointer to the child object
	  * @param offset Position offset inside the parent object
	  */
	void RemoveChild(Object* child);

	/** Call the user build function for this class
	  */
	void Build();

	/** Draw all sub-objects
	  * @param win Pointer to the GLFW window where the object geometry will be drawn
	  * @param proj Pointer to the current projection matrix
	  * @param view Pointer to the current camera view matrix
	  */
	void Draw(Window3d* win, Matrix4* proj, Matrix4* view);
	
protected:
	bool built; ///< Flag indicating that the geometry has been constructed

	bool bDrawOrigin; ///< Set if object origin point and local unit vectors will be drawn

	bool bDrawNormals; ///< Set if face normals will be drawn

	bool bHidden; ///< Set if object is hidden

	size_t reservedVertices; ///< The number of expected vertices

	size_t reservedPolygons; ///< The number of expected polygons which will be built

	Vector3 position; ///< The position offset of the object (not necessarily the center)

	Vector3 position0; ///< The original position offset of the object
	
	Matrix3 rotation; ///< The rotation of the object about the offset position

	Vector3 uX; ///< Unit vector for the x-axis

	Vector3 uY; ///< Unit vector for the y-axis

	Vector3 uZ; ///< Unit vector for the z-axis

	Vector3 center; ///< Center of the box which bounds the model

	Vector3 scaleFactor; ///< X, Y, and Z scale of object

	WrappedValue pitchAngle; ///< Angle of camera tilt up or down (i.e. about the horizontal axis)

	WrappedValue rollAngle; ///< Angle of camera roll cw or ccw (i.e. about the depth axis)

	WrappedValue yawAngle; ///< Angle of camera turn left or right (i.e. about the vertical axis

	float maxSize[3]; ///< Maximum extent along the x, y, and z-axes

	float minSize[3]; ///< Minimum extent along the x, y, and z-axes

	VertexContainer vertices; ///< Vector of all unique vertices
	
	PolygonContainer polys; ///< Vector of all unique polygons which make up this 3d object

	std::vector<Object*> children; ///< Vector of pointers to child objects

	const Object* parent; ///< Pointer to parent object

	const Shader3d* shader; ///< Pointer to an OpenGL shader to use for rendering

	const Texture* texture; ///< OpenGL texture

	ColorRGB ambientColor; ///< Ambient object color

	Matrix4 modelMatrix; ///< Model rotation and translation matrix used by OpenGL

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void SetSizeX(const float& min_, const float& max_);

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void SetSizeY(const float& min_, const float& max_);

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void SetSizeZ(const float& min_, const float& max_);

	/** Set this object's parent
	  * @return True if this object had no parent, and return false if otherwise
	  */
	bool SetParent(const Object* obj);

	/** Update the rotation of all child objects to that of the parent
	  */
	void UpdateRotation();

	/** Reserve space in the geometry vectors so that they will not resize when being filled
	  * @param nVert Number of expected vertices
	  * @param nPoly Number of expected polygons. If equal to zero, use the number of vertices
	  */
	void Reserve(const size_t& nVert, const size_t& nPoly=0);

	/** Reserve space in the vertex vector so that it will not resize when being filled
	  * @param nVert Number of expected vertices
	  */
	void ReserveVertices(const size_t& nVert);

	/** Reserve space in the polygon vector so that it will not resize when being filled
	  * @param nPoly Number of expected polygons
	  */
	void ReservePolygons(const size_t& nPoly);

	/** Rotate all vertices using the object's internal rotation matrix
	  */
	void Transform(const Matrix3* mat);
	
	/** Add a unique vertex to the vector of vertices and UV texture map coordinates (if specified)
	  * @param x X position of vertex
	  * @param y Y position of vertex
	  * @param z Z position of vertex
	  * @param u Horizontal UV texture map coordinate
	  * @param v Vertical UV texture map coordinate
	  * @return Pointer to the vertex vector
	  */
	Vertex* AddVertex(const float &x, const float& y, const float& z, const float& u=0.f, const float& v=0.f);

	/** Add a unique vertex to the vector of vertices and UV texture map coordinates (if specified)
	  * @param vec Position of vertex
	  * @param uv UV texture map coordinates
	  * @return Pointer to the vertex vector
	  */
	Vertex* AddVertex(const Vector3& vec, const Vector2& uv=Vector2(0.f, 0.f));

	/** Add a unique polygon to the vector of polygons
	  */
	void AddTriangle(const Vertex* v0, const Vertex* v1, const Vertex* v2);

	/** Add a unique polygon to the vector of polygons
	  */
	void AddTriangle(const uint16_t &i0, const uint16_t &i1, const uint16_t &i2);

	/** Add a unique polygon with UV texture mapping to the vector of polygons
	  */
	void AddTriangle(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2,
		const Vector2& uv0, const Vector2& uv1, const Vector2& uv2);

	/** Add two unique polygons (representing a quadrilateral) to the vector of polygons
	  */
	void AddQuad(const Vertex* v0, const Vertex* v1, const Vertex* v2, const Vertex* v3);

	/** Add two unique polygons (representing a quadrilateral) to the vector of polygons
	  */
	void AddQuad(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, const uint16_t& i3);

	/** Add two unique polygons with UV texture mapping (representing a textured quadrilateral) to the vector of polygons
	  */
	void AddQuad(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, const uint16_t& i3,
		const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3);

	/** Add a unique static triangle to the vector of polygons
	  * @note Static triangles cannot be moved or rotated
	  */
	void AddStaticTriangle(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2);

	/** Add two unique static triangles (representing a quadrilateral) to the vector of polygons
	  * @note Static triangles cannot be moved or rotated
	  */
	void AddStaticQuad(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, const uint16_t& i3);

	/** Build this object by adding polygons to the vector of polygons
	  */
	virtual void OnUserBuild() { };
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_OBJECT_HPP
