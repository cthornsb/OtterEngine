#ifndef LIGHT_SOURCE_HPP
#define LIGHT_SOURCE_HPP

#include "ColorRGB.hpp"
#include "ray.hpp"
#include "PixelTriplet.hpp"

class lightSource : public ray {
public:
	lightSource() : 
		ray(Vector3(0, 0, 0), Vector3(0, 0, 1)), 
		enabled(true),
		maxDistance(-1),
		brightness(1), 
		color(Colors::WHITE) 
	{ 
	}

	void enable() { enabled = true; }

	void disable() { enabled = false; }

	/** Get the brightness of the light source
	  */
	float getBrightness() const { return brightness; }
	
	/** Get the color of the light source
	  */
	ColorRGB getColor() const { return color; }
	
	/** Get the scaled color of the light source based on the angle between the direction of the
	  * light source and the normal to a surface
	  */
	ColorRGB getColor(const Vertex* vert, const Vector3& norm) const { return (color * getIntensity(vert, norm)); }

	/** Set the brightness of the light source
	  */
	void setBrightness(const float &brightness_){ brightness = brightness_; }

	/** Set the color of the light source
	  */
	void setColor(const ColorRGB &color_){ color = color_; }

	/** Check that the light source is enabled
	  */
	bool isEnabled() const { return enabled; }

protected:
	bool enabled; ///< Light enable flag

	float maxDistance; ///< Maximum effective distance of light source

	float brightness; ///< The brightness of the light source

	ColorRGB color; ///< The color of the light source
	
	/** Get the intensity scaling factor based on the angle between the direction of the
	  * light source and the normal to a surface
	  */
	virtual float getIntensity(const Vertex* vert, const Vector3& norm) const ;
};

class directionalLight : public lightSource {
public:
	/** Default constructor
	  */
	directionalLight() : lightSource() { }
};

class pointLight : public lightSource {
public:
	/** Default constructor
	  */
	pointLight() : lightSource() { }

protected:
	/** Get the intensity scaling factor based on the angle between the direction of the
	  * light source and the normal to a surface as well as the distance from the point
	  * to the light position
	  */
	float getIntensity(const Vertex* vert, const Vector3& norm) const ;
};

class coneLight : public lightSource {
public:
	/** Default constructor
	  */
	coneLight() : lightSource(), openingAngle(0.5236f) { }
	
	void setOpeningAngle(const float& angle) { openingAngle = angle; }

protected:
	float openingAngle; ///< The opening angle of the light cone (in radians)

	/** Get the intensity scaling factor based on the angle between the direction of the
	  * light source and the normal to a surface as well as the distance from the test point
	  * to the light position. The angle between the test point and the direction of the cone
	  * is checked against the opening angle of the light source, with any points lying outside
	  * the cone being given an intensity of zero
	  */
	float getIntensity(const Vertex* vert, const Vector3& norm) const ;

	/** Compute the fractional intensity based on the angle from the directional axis of the light (radians)
	  */
	virtual float radialIntensity(const float& angle) const;
};

#endif
