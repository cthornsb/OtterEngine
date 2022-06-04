#ifndef GRAPHICS_GEOMETRY_OTT_PRIMITIVES_HPP
#define GRAPHICS_GEOMETRY_OTT_PRIMITIVES_HPP

#include "OTTObject.hpp"

namespace ott {
namespace primitives {

typedef Vector3 (*VertexPositionFunction_t)(const float& px, const float& py, const float& pz);

Vector3 DefaultVertexPosition(const float& px, const float& py, const float& pz);

class Plane : public Object {
public:
	Plane() :
		Object(),
		dX(0.5),
		dY(0.5)
	{
	}

	Plane(const Vector3& pos_) :
		Object(pos_),
		dX(0.5),
		dY(0.5)
	{
	}

	Plane(const Vector3& pos_, const float& X, const float& Y) :
		Object(pos_),
		dX(X/2),
		dY(Y/2)
	{
	}

	/** Build this Object by adding polygons to the vector of polygons
		*/
	void OnUserBuild() override;

protected:
	float dX; ///< Half width of plane along the x-axis
	float dY; ///< Half height of plane along the y-axis
};

class Cube : public Plane {
public:
	Cube() :
		Plane(),
		dZ(0.5)
	{
	}

	Cube(const Vector3& pos_) :
		Plane(pos_),
		dZ(0.5)
	{
	}

	Cube(const Vector3& pos_, const float& X, const float& Y, const float& Z) :
		Plane(pos_, X, Y),
		dZ(Z/2)
	{
	}

	/** Build this Object by adding polygons to the vector of polygons
		*/
	void OnUserBuild() override;

protected:
	float dZ; ///< Half depth of cube along the z-axis
};

class Circle : public Object {
public:
	Circle() :
		Object(),
		dR(0.5),
		nVertices(6)
	{
	}

	Circle(const Vector3& pos_) :
		Object(pos_),
		dR(0.5),
		nVertices(6)
	{
	}

	Circle(const Vector3& pos_, const float& R, const int32_t& sides) :
		Object(pos_),
		dR(R),
		nVertices(sides)
	{
	}

	/** Build this Object by adding polygons to the vector of polygons
		*/
	void OnUserBuild() override;

protected:
	float dR; ///< Radius of the circle

	int32_t nVertices; ///< Number of vertices used to approximate the circumference of the circle (not counting the central vertex)
};

class Cylinder : public Circle {
public:
	Cylinder() :
		Circle(),
		dZ(0.5)
	{
	}

	Cylinder(const Vector3& pos_) :
		Circle(pos_),
		dZ(0.5)
	{
	}

	Cylinder(const Vector3& pos_, const float& R, const float& Z, const int32_t& sides) :
		Circle(pos_, R, sides),
		dZ(Z/2)
	{
	}

	/** Build this Object by adding polygons to the vector of polygons
		*/
	void OnUserBuild() override;

protected:
	float dZ; ///< Half depth of the cylinder along the x-axis
};

class Cone : public Circle {
public:
	Cone() :
		Circle(),
		dZ(0.5)
	{
	}

	Cone(const Vector3& pos_) :
		Circle(pos_),
		dZ(0.5)
	{
	}

	Cone(const Vector3& pos_, const float& R, const float& Z, const int32_t& sides) :
		Circle(pos_, R, sides),
		dZ(Z / 2)
	{
	}

	/** Build this Object by adding polygons to the vector of polygons
		*/
	void OnUserBuild() override;

private:
	float dZ; ///< Half height of the cylinder along the z-axis
};

class Sphere : public Object {
public:
	Sphere() :
		Object(),
		dR(0.5f),
		nDivLatitude(8),
		nDivLongitude(8)
	{
	}

	Sphere(const Vector3& pos_) :
		Object(pos_),
		dR(0.5f),
		nDivLatitude(8),
		nDivLongitude(8)
	{
	}

	Sphere(const Vector3& pos_, const float& R, const int32_t& divLat=8, const int32_t& divLong=8) :
		Object(pos_),
		dR(R / 2),
		nDivLatitude(divLat),
		nDivLongitude(divLong)
	{
	}

	/** Build this Object by adding polygons to the vector of polygons
		*/
	void OnUserBuild() override;

protected:
	float dR;

	int32_t nDivLatitude;
	int32_t nDivLongitude;

	Vector2 ComputeUV(const Vector3& surfPos) const;

	void AddLatitudePoints(const float& dR, const float& dY, std::vector<Vector2>& uvCoords);
};

class Mesh : public Object {
public:
	/** Default constructor
		*/
	Mesh() :
		Object(),
		dX(10.f),
		dZ(10.f),
		nDivX(10),
		nDivZ(10),
		pfunc(DefaultVertexPosition)
	{
	}

	/** Default constructor
		*/
	Mesh(const Vector3& pos_) :
		Object(pos_),
		dX(10.f),
		dZ(10.f),
		nDivX(10),
		nDivZ(10),
		pfunc(DefaultVertexPosition)
	{
	}

	/** Square mesh constructor
		*/
	Mesh(const Vector3& pos_, const float& length, const int32_t& div, VertexPositionFunction_t fptr = DefaultVertexPosition) :
		Object(pos_),
		dX(length),
		dZ(length),
		nDivX(div),
		nDivZ(div),
		pfunc(fptr)
	{
	}

	/** Rectangular mesh constructor
		*/
	Mesh(const Vector3& pos_, const float& X, const float& Z, const int32_t& divX, const int32_t& divZ, VertexPositionFunction_t fptr = DefaultVertexPosition) :
		Object(pos_),
		dX(X),
		dZ(Z),
		nDivX(divX),
		nDivZ(divZ),
		pfunc(fptr)
	{
	}

	/** Build this Object by adding polygons to the vector of polygons
		*/
	void OnUserBuild() override ;

private:
	float dX; ///< Length of mesh plane along the x-axis
	float dZ; ///< Length of mesh plane along the z-axis

	int32_t nDivX; ///< Number of divisions along the x-axis
	int32_t nDivZ; ///< Number of divisions along the z-axis

	VertexPositionFunction_t pfunc;
};

} /* namespace primitives */
} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_PRIMITIVES_HPP
