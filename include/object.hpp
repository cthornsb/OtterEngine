#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "scene.hpp"
#include "Matrix.hpp"
#include "triangle.hpp"
#include "Vertex.hpp"
#include "WrappedValue.hpp"
#include "VertexContainer.hpp"
#include "PolygonContainer.hpp"

class camera;
class Shader;
class Texture;

/// <summary>
/// 3d object geometry class
/// </summary>
class object{
public:
	/** Default constructor
	  */
	object();

	/** Object position constructor
	  */	
	object(const Vector3& pos_);
	
	/** Get a pointer to the vector of polygons which comprise this 3d object
	  */
	std::vector<triangle>* getPolygonVector() { return polys.getPolygons(); }

	/** Get the position offset of the object
	  */
	Vector3 getPosition() const { return position; }

	/** Get the position of the center of the object
	  */
	Vector3 getCenter() const { return (center + position); }

	/** Get the size of the object along the x-axis
	  */
	float getSizeX() const { return (maxSize[0] - minSize[0]); }

	/** Get the size of the object along the y-axis
	  */
	float getSizeY() const { return (maxSize[1] - minSize[1]); }

	/** Get the size of the object along the z-axis
	  */
	float getSizeZ() const { return (maxSize[2] - minSize[2]); }

	/** Get the current pitch angle (angle about the x-axis) (in radians)
	  */
	float getPitchAngle() const { return pitchAngle.get(); }

	/** Get the current roll angle (angle about the z-axis) (in radians)
	  */
	float getRollAngle() const { return rollAngle.get(); }

	/** Get the current yaw angle (angle about the y-axis) (in radians)
	  */
	float getYawAngle() const { return yawAngle.get(); }

	/** Get a const pointer to the position vector
	  */
	const Vector3* getConstPositionPointer() const { return (const Vector3*)&position; }

	/** Get a const pointer to the position vector
	  */
	Vector3* getPositionPointer() { return &position; }

	/** Get a const pointer to the rotation matrix
	  */
	const Matrix3* getConstRotationMatrix() const { return (const Matrix3*)&rotation; }

	/** Get a pointer to the rotation matrix
	  */
	Matrix3* getRotationMatrix() { return &rotation; }

	/** Get the number of unique vertices
	  */
	size_t getNumberOfVertices() const { return vertices.size(); }
	
	/** Get the number of unique polygons
	  */
	size_t getNumberOfPolygons() const { return polys.size(); }

	/** Get the number of expected vertices
	  */
	size_t getNumberOfReservedVertices() const { return reservedVertices; }

	/** Get the number of expected polygons
	  */
	size_t getNumberOfReservedPolygons() const { return reservedPolygons; }

	/** Get a const pointer to this object's parent
	  * @return Pointer to this object's parent object, or (this) if object has no parent
	  */
	const object* getParent() const { return (parent ? parent : this); }

	/** Get a const pointer to the shader used to render the object
	  */
	const Shader* getShader() const { return shader; }

	/** Get a pointer to the vertex container object
	  */
	const VertexContainer* getVertices() const { return &vertices; }

	/** Get a pointer to the polygon container object
	  */
	const PolygonContainer* getConstPolygonContainer() const { return &polys; }

	/** Get OpenGL VBO id number corresponding to object vertices
	  */
	unsigned int getVertexVBO() const { return polys.getVertexVBO(); }

	/** Get OpenGL VBO id number corresponding to object vertex indicies
	  */
	unsigned int getIndexVBO() const { return polys.getIndexVBO(); }

	/** Get the OpenGL modelview matrix
	  */
	Matrix4* getModelMatrix();

	/** Get the OpenGL texture ID
	  */
	unsigned int getTexture() const { return textureID; }

	/** Return true if this object has a parent object and return false otherwise
	  */
	bool isChild() const { return (parent != 0x0); }

	/** Return true if this object has child objects and return false otherwise
	  */
	bool hasChildren() const { return !children.empty(); }

	/** Get const iterator at the start of the child objects vector
	  */
	std::vector<object*>::const_iterator beginChildren() const { return children.cbegin(); }

	/** Get const iterator at the end of the child objects vector
	  */
	std::vector<object*>::const_iterator endChildren() const { return children.cend(); }

	/** Rotate the object by a given amount about the X, Y, and Z, axes (all in radians)
	  * @note This method will rotate vertices from their current position. Use setRotation() to specify the rotation explicitly
	  */
	void rotate(const float& theta, const float& phi, const float& psi);

	/** Move the position of the object
	  * @note This method moves the object relative to its current position. Use setPosition() to specify the position explicitly
	  */
	void move(const Vector3 &offset);

	/** Rotate the object to specified angles about the X, Y, and Z, axes (all in radians)
	  */
	void setRotation(const float& theta, const float& phi, const float& psi);

	/** Set the position of the object
	  */
	void setPosition(const Vector3 &pos);

	/** Set the shader to use for rendering
	  */
	void setShader(const Shader* shdr);

	/** Scale object relative to its current size
	  */
	void scale(const float& scale_) { scaleFactor *= scale_; }

	/** Set scale of all three vector axes
	  */
	void setScale(const float& scale_) { scaleFactor = Vector3(scale_, scale_, scale_); }

	/** Set scale of all three vector axes
	  */
	void setScale(const Vector3& scale_) { scaleFactor = scale_; }

	/** Set scale of all three vector axes
	  */
	void setScale(const float& xScale, const float& yScale, const float& zScale) { scaleFactor = Vector3(xScale, yScale, zScale); }

	/** Set scale of object's x-axis
	  */
	void setScaleX(const float& scale_) { scaleFactor[0] = scale_; }

	/** Set scale of object's y-axis
	  */
	void setScaleY(const float& scale_) { scaleFactor[1] = scale_; }

	/** Set scale of object's z-axis
	  */
	void setScaleZ(const float& scale_) { scaleFactor[2] = scale_; }

	/** Set object's texture
	  */
	void setTexture(Texture* txt);

	/** Reset the offset position of the object to its original location
	  */
	void resetPosition();

	/** Project all vertices onto the view-plane
	  */
	void renderAllVertices(camera* cam);

	/** Add a child to this object
	  * @note An object with multiple parents may cause undefined behavior
	  * @param child Pointer to the child object
	  * @param offset Position offset inside the parent object
	  */
	void addChild(object* child, const Vector3& offset = zeroVector);

	/** Remove a child from this object
	  * @param child Pointer to the child object
	  * @param offset Position offset inside the parent object
	  */
	void removeChild(object* child);

	/** Call the user build function for this class
	  */
	void build();

	/** Build this object by adding polygons to the vector of polygons
	  */
	virtual void userBuild() { };

	/** Draw all sub-objects
	  */
	void draw(Window* win);
	
protected:
	bool built; ///< Flag indicating that the geometry has been constructed

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

	std::vector<object*> children; ///< Vector of pointers to child objects

	Vector3 parentOffset; ///< Offset of this object within its parent

	const object* parent; ///< Pointer to parent object

	const Shader* shader; ///< Pointer to an OpenGL shader to use for rendering

	Matrix4 modelMatrix; ///< Model rotation and translation matrix used by OpenGL

	unsigned int textureID; ///< OpenGL texture ID

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void setSizeX(const float& min_, const float& max_);

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void setSizeY(const float& min_, const float& max_);

	/** Update the physical size of the object along the x-axis
	  * @note The size is only for informational purposes and does not change the scale of the object
	  */
	void setSizeZ(const float& min_, const float& max_);

	/** Set this object's parent
	  * @return True if this object had no parent, and return false if otherwise
	  */
	bool setParent(const object* obj);

	/** Update the position of all child objects to the parent position
	  */
	void updatePosition();

	/** Update the position based on new parent position
	  */
	void updatePositionForParent(const Vector3& pos);

	/** Update the rotation of all child objects to that of the parent
	  */
	void updateRotation();

	/** Update the rotation based on new parent rotation
	  */
	void updateRotationForParent(const Matrix3* rot);

	/** Reserve space in the geometry vectors so that they will not resize when being filled
	  * @param nVert Number of expected vertices
	  * @param nPoly Number of expected polygons. If equal to zero, use the number of vertices
	  */
	void reserve(const size_t& nVert, const size_t& nPoly=0);

	/** Reserve space in the vertex vector so that it will not resize when being filled
	  * @param nVert Number of expected vertices
	  */
	void reserveVertices(const size_t& nVert);

	/** Reserve space in the polygon vector so that it will not resize when being filled
	  * @param nPoly Number of expected polygons
	  */
	void reservePolygons(const size_t& nPoly);

	/** Rotate all vertices using the object's internal rotation matrix
	  */
	void transform(const Matrix3* mat);
	
	/** Add a unique vertex to the vector of vertices and UV texture map coordinates (if specified)
	  * @param x X position of vertex
	  * @param y Y position of vertex
	  * @param z Z position of vertex
	  * @param u Horizontal UV texture map coordinate
	  * @param v Vertical UV texture map coordinate
	  * @return Pointer to the vertex vector
	  */
	Vertex* addVertex(const float &x, const float& y, const float& z, const float& u=0.f, const float& v=0.f);

	/** Add a unique vertex to the vector of vertices and UV texture map coordinates (if specified)
	  * @param vec Position of vertex
	  * @param uv UV texture map coordinates
	  * @return Pointer to the vertex vector
	  */
	Vertex* addVertex(const Vector3& vec, const Vector2& uv=Vector2(0.f, 0.f));

	/** Add a unique polygon to the vector of polygons
	  */
	void addTriangle(Vertex* v0, Vertex* v1, Vertex* v2);

	/** Add a unique polygon to the vector of polygons
	  */
	void addTriangle(const unsigned short &i0, const unsigned short &i1, const unsigned short &i2);

	/** Add a unique polygon with UV texture mapping to the vector of polygons
	  */
	void addTriangle(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2,
		const Vector2& uv0, const Vector2& uv1, const Vector2& uv2);

	/** Add two unique polygons (representing a quadrilateral) to the vector of polygons
	  */
	void addQuad(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3);

	/** Add two unique polygons (representing a quadrilateral) to the vector of polygons
	  */
	void addQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3);

	/** Add two unique polygons with UV texture mapping (representing a textured quadrilateral) to the vector of polygons
	  */
	void addQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3,
		const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3);

	/** Add a unique static triangle to the vector of polygons
	  * @note Static triangles cannot be moved or rotated
	  */
	void addStaticTriangle(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2);

	/** Add two unique static triangles (representing a quadrilateral) to the vector of polygons
	  * @note Static triangles cannot be moved or rotated
	  */
	void addStaticQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3);
};

/// <summary>
/// Sub-object of 3d object used to organize geometry groups
/// </summary>
class SubObject : public object {
public:
	SubObject() :
		object(),
		parent(0x0)
	{
	}

	SubObject(object* parent_) :
		object(),
		parent(parent_)
	{
	}

	void addSubGeometry(Vertex* v0, Vertex* v1, Vertex* v2);

	void addSubGeometry(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3);

	PolygonContainer* getPolygonContainer() { return &polys; }

private:
	object* parent;
};

#endif
