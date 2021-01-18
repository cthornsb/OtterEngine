#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "Vector.hpp"
#include "colors.hpp"

class Matrix3;
class object;

class Vertex {
public:
	friend class object;
	friend class triangle;

	short pX; ///< The horizontal pixel coordinates of the vertex projection
	short pY; ///< The vertical pixel coordinates of the vertex projection

	float sX; ///< Horizontal screen coordinates of the vertex projection on the camera view-plane [-1, 1]
	float sY; ///< Vertical screen coordinates of the vertex projection on the camera view-plane [-1, 1]

	float zDepth; ///< The distance between the vertex and the focal point of the camera, into the screen

	Vertex() :
		pX(0),
		pY(0),
		sX(0),
		sY(0),
		zDepth(0),
		pos0(),
		norm0(),
		color(1.f, 1.f, 1.f),
		texCoords(0.f, 0.f),
		offset(&zeroVector),
		rotation(0x0),
		parent(0x0)
	{
	}

	Vertex(const Vector3& position, const object* obj);

	bool operator == (const Vector3& vec) const { return (vec == pos0); }

	/** Permanently offset the vertex position by an input vector
	  */
	void offsetPosition(const Vector3& vec) { pos0 += vec; }

	/** Get the position of this vertex
	  */
	Vector3 getPosition() const;

	/** Get the initial vertex position
	  */
	Vector3 getInitialPosition() const { return pos0; }

	/** Get the normal vector at this vertex
	  */
	Vector3 getNormal() const;

	/** Get the initial vertex normal
	  */
	Vector3 getInitialNormal() const { return norm0; }

	/** Get the vertex color
	  */
	ColorRGB getColor() const { return color; }

	/** Get the vertex texture coordinates
	  */
	Vector2 getTextureCoordinates() const { return texCoords; }

	/** Set the vertex color
	  */
	void setColor(const ColorRGB& col) { color = col; }

	/** Set the vertex texture coordinates
	  */
	void setTextureCoordinates(const Vector2& texture) { texCoords = texture; }

	/** Transform the vertex position by an input matrix and return the result
	  */
	Vector3 transform(const Matrix3* mat);

private:
	Vector3 pos0; ///< Original vertex position

	Vector3 norm0; ///< Vertex normal vector

	ColorRGB color; ///< Vertex color

	Vector2 texCoords; ///< Texture coordinates

	const Vector3* offset; ///< Vertex position offset

	const Matrix3* rotation; ///< Rotation matrix of object

	const object* parent; ///< Pointer to the parent object
};

#endif
