#ifndef PIXEL_TRIPLET_HPP
#define PIXEL_TRIPLET_HPP

namespace ott {

struct Ray_t;
class Vertex;

class LightSource;
class PixelTriplet;
class Polygon;

class DepthCalc {
public:
	DepthCalc() :
		A(0),
		B(0),
		C(0)
	{
	}

	void Setup(const float* sX, const float* sY, const float* zDepth);

	void Setup(const PixelTriplet& pixels);

	void Setup(const PixelTriplet& pixels, const float& z0, const float& z1, const float& z2);

	float Z(const float& x, const float& y) const;

private:
	float A;
	float B;
	float C;

	void Compute(
		const float& x0, const float& y0, const float& z0,
		const float& x1, const float& y1, const float& z1,
		const float& x2, const float& y2, const float& z2);
};

/** @class pixelTriplet
  * @brief Simple holder for the pixel coordinates of a 2d projection of a 3d triangle
  * @author Cory R. Thornsberry
  * @date September 5, 2019
  */
class PixelTriplet {
public:
	Polygon* tri; ///< Pointer to the real triangle

	const Vertex* p0;
	const Vertex* p1;
	const Vertex* p2;

	DepthCalc zCalc;

	DepthCalc rCalc;
	DepthCalc gCalc;
	DepthCalc bCalc;

	ColorRGB light0; ///< Dynamic lighting color at vertex 0
	ColorRGB light1; ///< Dynamic lighting color at vertex 1
	ColorRGB light2; ///< Dynamic lighting color at vertex 2

	bool draw[3]; ///< Flag for each vertex indicating that it is on the screen

	/** Default constructor
	  */
	PixelTriplet();

	/** Constructor taking a pointer to a 3d triangle
	  */
	PixelTriplet(Polygon* t);

	const Vertex* operator [] (const size_t& index) const;

	/** Return true if at least one of the vertices is on the screen and return false otherwise
	  */
	bool GoodToDraw() const { 
		return (draw[0] || draw[1] || draw[2]); 
	}

	Vector3 CenterPoint() const;

	ColorRGB LightColor(const float& x, const float& y) const;

	float ZDepth(const float& x, const float& y) const { 
		return zCalc.Z(x, y); 
	}

	void Set(const size_t& index, const int32_t& x, const int32_t& y);

	bool SortVertical(const int32_t& yMax);

	bool HorizontalLimits(const int32_t& scanline, int32_t& xA, int32_t& xB) const;

	void ComputeVertexLighting(LightSource* light);

	void Finalize();

	/** Check if a ray intersects with the infinite plane defined by this object
	  * @param ray The ray to check for intersection
	  * @param t The "distance" from the ray's vertex point, along its direction, to the point of intersection on the plane
	  * @return True if the plane is in front of the ray, and return false if the plane is behind the ray (i.e. they do not intersect)
	  */
	bool Intersects(const Ray_t& ray, float& t) const;
};

} /* namespace ott */

#endif
