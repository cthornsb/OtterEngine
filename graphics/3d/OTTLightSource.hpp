#ifndef GRAPHICS_3D_OTT_LIGHT_SOURCE_HPP
#define GRAPHICS_3D_OTT_LIGHT_SOURCE_HPP

#include <graphics/core/OTTColor.hpp>
#include <graphics/geometry/OTTRay.hpp>

namespace ott {

enum class LightType {
	Disabled,
	Directional,
	Point,
	Cone
};

class Vertex;

class LightSource {
public:
	LightSource() : 
		ray(Vector3(0, 0, 0), Vector3(0, 0, 1)), 
		enabled(true),
		maxDistance(-1),
		brightness(1), 
		color(colors::White) 
	{ 
	}

	void Enable() {
		enabled = true; 
	}

	void Disable() {
		enabled = false; 
	}

	/** Get the brightness of the light source
	  */
	float Brightness() const {
		return brightness; 
	}
	
	/** Get the color of the light source
	  */
	ColorRGB Color() const {
		return color; 
	}
	
	/** Get the scaled color of the light source based on the angle between the direction of the
	  * light source and the normal to a surface
	  */
	ColorRGB Color(const Vertex* vert, const Vector3& norm) const {
		return (color * this->Intensity(vert, norm)); 
	}

	/** Set the brightness of the light source
	  */
	void SetBrightness(const float &brightness_) { 
		brightness = brightness_; 
	}

	/** Set the color of the light source
	  */
	void SetColor(const ColorRGB &color_) { 
		color = color_; 
	}

	/** Check that the light source is enabled
	  */
	bool Enabled() const {
		return enabled; 
	}

protected:
	Ray_t ray;

	bool enabled; ///< Light enable flag

	float maxDistance; ///< Maximum effective distance of light source

	float brightness; ///< The brightness of the light source

	ColorRGB color; ///< The color of the light source
	
	/** Get the intensity scaling factor based on the angle between the direction of the
	  * light source and the normal to a surface
	  */
	virtual float Intensity(const Vertex* vert, const Vector3& norm) const ;
};

class DirectionalLight : public LightSource {
public:
	/** Default constructor
	  */
	DirectionalLight() : LightSource() {
	}
};

class PointLight : public LightSource {
public:
	/** Default constructor
	  */
	PointLight() : LightSource() {
	}

protected:
	/** Get the intensity scaling factor based on the angle between the direction of the
	  * light source and the normal to a surface as well as the distance from the point
	  * to the light position
	  */
	float Intensity(const Vertex* vert, const Vector3& norm) const ;
};

class ConeLight : public LightSource {
public:
	/** Default constructor
	  */
	ConeLight() : 
		LightSource(), 
		openingAngle(0.5236f) 
	{
	}
	
	void SetOpeningAngle(const float& angle) {
		openingAngle = angle; 
	}

protected:
	float openingAngle; ///< The opening angle of the light cone (in radians)

	/** Get the intensity scaling factor based on the angle between the direction of the
	  * light source and the normal to a surface as well as the distance from the test point
	  * to the light position. The angle between the test point and the direction of the cone
	  * is checked against the opening angle of the light source, with any points lying outside
	  * the cone being given an intensity of zero
	  */
	float Intensity(const Vertex* vert, const Vector3& norm) const ;

	/** Compute the fractional intensity based on the angle from the directional axis of the light (radians)
	  */
	virtual float RadialIntensity(const float& angle) const;
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_3D_OTT_LIGHT_SOURCE_HPP
