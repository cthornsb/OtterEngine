#ifndef PIXEL_TRIPLET_HPP
#define PIXEL_TRIPLET_HPP

#include "vector3.hpp"
#include "ray.hpp"

class triangle;
class camera;
class pixelTriplet;

class zDepthCalc {
public:
	zDepthCalc() :
		A(0),
		B(0),
		C(0)
	{
	}

	void setup(const float* sX, const float* sY, const float* zDepth);

	void setup(const pixelTriplet* pixels);

	float getZ(const float& x, const float& y) const;

private:
	float A;
	float B;
	float C;

	void compute(const float& x0, const float& y0, const float& z0,
		const float& x1, const float& y1, const float& z1,
		const float& x2, const float& y2, const float& z2);
};

/** @class pixelTriplet
  * @brief Simple holder for the pixel coordinates of a 2d projection of a 3d triangle
  * @author Cory R. Thornsberry
  * @date September 5, 2019
  */
class pixelTriplet {
public:
	const triangle* tri; ///< Pointer to the real triangle

	const vector3* offset; ///< Pointer to the offset vector of the parent object

	int pX[3]; ///< The horizontal pixel coordinates for the three vertices
	int pY[3]; ///< The vertical pixel coordinates for the three vertices

	float sX[3];
	float sY[3];

	float zDepth[3];

	zDepthCalc calc;

	bool draw[3]; ///< Flag for each vertex indicating that it is on the screen

	/** Default constructor
	  */
	pixelTriplet() :
		tri(0x0),
		offset(0x0),
		pX(),
		pY(),
		sX(),
		sY(),
		zDepth(),
		draw()
	{
	}

	/** Constructor taking a pointer to a 3d triangle
	  */
	pixelTriplet(const triangle* t) :
		tri(t),
		offset(0x0),
		pX(),
		pY(),
		sX(),
		sY(),
		zDepth(),
		draw()
	{
	}

	vector3 getVertex0() const;

	vector3 getVertex1() const;

	vector3 getVertex2() const;

	/** Return true if at least one of the vertices is on the screen and return false otherwise
	  */
	bool goodToDraw() const { return (draw[0] || draw[1] || draw[2]); }

	vector3 getCenterPoint() const;

	void set(const size_t& index, const int& x, const int& y);

	bool sortVertical(const int& yMax);

	bool getHorizontalLimits(const int& scanline, int& xA, int& xB) const;

	void finalize();

	/** Check if a ray intersects with the infinite plane defined by this object
	  * @param ray The ray to check for intersection
	  * @param t The "distance" from the ray's vertex point, along its direction, to the point of intersection on the plane
	  * @return True if the plane is in front of the ray, and return false if the plane is behind the ray (i.e. they do not intersect)
	  */
	bool intersects(const ray& ray, float& t) const;
};

#endif