#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "vector3.hpp"
#include "colors.hpp"

class matrix3;

class Vertex {
public:
	friend class object;

	vector3 pos; ///< Current vertex position

	vector3 norm; ///< Vertex normal vector

	short pX; ///< The horizontal pixel coordinates of the vertex projection
	short pY; ///< The vertical pixel coordinates of the vertex projection

	float sX; ///< Horizontal screen coordinates of the vertex projection on the camera view-plane [-1, 1]
	float sY; ///< Vertical screen coordinates of the vertex projection on the camera view-plane [-1, 1]

	float zDepth; ///< The distance between the vertex and the focal point of the camera, into the screen

	ColorRGB light; ///< Additive lighting color at vertex

	Vertex() :
		pos(),
		pos0(),
		norm(),
		offset(&zeroVector),
		pX(0),
		pY(0),
		sX(0),
		sY(0),
		zDepth(0),
		light()
	{
	}

	Vertex(const vector3& position, const vector3* displacement) :
		pos(position),
		pos0(position),
		norm(),
		offset(displacement),
		pX(0),
		pY(0),
		sX(0),
		sY(0),
		zDepth(0),
		light()
	{
	}

	bool operator == (const vector3& vec) const { return (vec == pos); }

	vector3 getPosition() const { return (pos + *offset); }

	void transform(const matrix3* mat);

	/** Reset position and normal to initial values
	  */
	void reset();

private:
	vector3 pos0; ///< Original vertex position

	const vector3* offset; ///< Vertex position offset
};

#endif