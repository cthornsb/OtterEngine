#ifndef PIXEL_TRIPLET_HPP
#define PIXEL_TRIPLET_HPP

#include "Vector.hpp"
#include "colors.hpp"
#include "ray.hpp"

class triangle;
class camera;
class pixelTriplet;
class lightSource;
class Vertex;

class zDepthCalc {
public:
	zDepthCalc() :
		A(0),
		B(0),
		C(0)
	{
	}

	void setup(const float* sX, const float* sY, const float* zDepth);

	void setup(const pixelTriplet& pixels);

	void setup(const pixelTriplet& pixels, const float& z0, const float& z1, const float& z2);

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
	triangle* tri; ///< Pointer to the real triangle

	Vertex* p0;
	Vertex* p1;
	Vertex* p2;

	zDepthCalc zCalc;

	zDepthCalc rCalc;
	zDepthCalc gCalc;
	zDepthCalc bCalc;

	ColorRGB light0; ///< Dynamic lighting color at vertex 0
	ColorRGB light1; ///< Dynamic lighting color at vertex 1
	ColorRGB light2; ///< Dynamic lighting color at vertex 2

	bool draw[3]; ///< Flag for each vertex indicating that it is on the screen

	/** Default constructor
	  */
	pixelTriplet();

	/** Constructor taking a pointer to a 3d triangle
	  */
	pixelTriplet(triangle* t);

	Vertex* operator [] (const size_t& index) const;

	/** Return true if at least one of the vertices is on the screen and return false otherwise
	  */
	bool goodToDraw() const { return (draw[0] || draw[1] || draw[2]); }

	Vector3 getCenterPoint() const;

	ColorRGB getLighting(const float& x, const float& y) const;

	float getZDepth(const float& x, const float& y) const { return zCalc.getZ(x, y); }

	void set(const size_t& index, const int& x, const int& y);

	bool sortVertical(const int& yMax);

	bool getHorizontalLimits(const int& scanline, int& xA, int& xB) const;

	void computeVertexLighting(lightSource* light);

	void finalize();

	/** Check if a ray intersects with the infinite plane defined by this object
	  * @param ray The ray to check for intersection
	  * @param t The "distance" from the ray's vertex point, along its direction, to the point of intersection on the plane
	  * @return True if the plane is in front of the ray, and return false if the plane is behind the ray (i.e. they do not intersect)
	  */
	bool intersects(const ray& ray, float& t) const;
};

#endif