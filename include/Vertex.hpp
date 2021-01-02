#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "vector3.hpp"
#include "colors.hpp"

class matrix3;

class Vertex {
public:
	friend class object;
	friend class triangle;

	float* p0; ///< Pointer to the x-coordinate of the vertex position
	float* p1; ///< Pointer to the y-coordinate of the vertex position
	float* p2; ///< Pointer to the z-coordinate of the vertex position

	short pX; ///< The horizontal pixel coordinates of the vertex projection
	short pY; ///< The vertical pixel coordinates of the vertex projection

	float sX; ///< Horizontal screen coordinates of the vertex projection on the camera view-plane [-1, 1]
	float sY; ///< Vertical screen coordinates of the vertex projection on the camera view-plane [-1, 1]

	float zDepth; ///< The distance between the vertex and the focal point of the camera, into the screen

	Vertex() :
		p0(0x0),
		p1(0x0),
		p2(0x0),
		pX(0),
		pY(0),
		sX(0),
		sY(0),
		zDepth(0),
		pos0(),
		norm0(),
		offset(&zeroVector),
		rotation(0x0),
		parent(0x0)
	{
	}

	Vertex(const vector3& position, const object* obj);

	bool operator == (const vector3& vec) const { return (vec == pos0); }

	/** Permanently offset the vertex position by an input vector
	  */
	void offsetPosition(const vector3& vec) { pos0 += vec; }

	/** Get the position of this vertex
	  */
	vector3 getPosition() const;

	/** Get the normal vector at this vertex
	  */
	vector3 getNormal() const;

	/** Transform the vertex position by an input matrix and return the result
	  */
	vector3 transform(const matrix3* mat);

private:
	vector3 pos0; ///< Original vertex position

	vector3 norm0; ///< Vertex normal vector

	const vector3* offset; ///< Vertex position offset

	const matrix3* rotation; ///< Rotation matrix of object

	const object* parent; ///< Pointer to the parent object
};

#endif