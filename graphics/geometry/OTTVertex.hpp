#ifndef GRAPHICS_GEOMETRY_OTT_VERTEX_HPP
#define GRAPHICS_GEOMETRY_OTT_VERTEX_HPP

#include <math/OTTVector.hpp>
#include <graphics/core/OTTColor.hpp>

namespace ott {

class Matrix3;
class Object;

class Vertex {
public:
#if defined(SOFTWARE_RENDERER)
	int16_t pX; ///< The horizontal pixel coordinates of the vertex projection
	int16_t pY; ///< The vertical pixel coordinates of the vertex projection

	float sX; ///< Horizontal screen coordinates of the vertex projection on the camera view-plane [-1, 1]
	float sY; ///< Vertical screen coordinates of the vertex projection on the camera view-plane [-1, 1]

	float zDepth; ///< The distance between the vertex and the focal point of the camera, into the screen
#endif // #if defined(SOFTWARE_RENDERER)

	Vector3 pos0; ///< Original vertex position
	Vector3 norm0; ///< Vertex normal vector

	ColorRGB color; ///< Vertex color

	Vector2 texCoords; ///< Texture coordinates

	const Vector3* offset;
	const Matrix3* rotation;

	Vertex() :
#if defined(SOFTWARE_RENDERER)
		pX(0),
		pY(0),
		sX(0),
		sY(0),
		zDepth(0),
#endif // #if defined(SOFTWARE_RENDERER)
		pos0(),
		norm0(),
		color(1.f, 1.f, 1.f),
		texCoords(0.f, 0.f),
		offset(nullptr),
		rotation(nullptr)
	{
	}

	Vertex(const Vector3& position, const Object* obj);

	Vertex(const Vector3& position, const Vector3& normal, const Object* obj);

	bool operator == (const Vector3& vec) const { 
		
		return (vec == pos0); 
	}

	/** Permanently offset the vertex position by an input vector
	  */
	void Offset(const Vector3& vec) { 
		
		pos0 += vec; 
	}

	/** Get the position of this vertex
	  */
	Vector3 Position() const;

	/** Get the normal vector at this vertex
	  */
	Vector3 Normal() const;

	/** Transform the vertex position by an input matrix and return the result
	  */
	Vector3 Transform(const Matrix3* mat);
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_VERTEX_HPP
