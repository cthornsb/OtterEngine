#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "Texture.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"

class Sprite : public Texture {
public:
	/** Default constructor
	  **/
	Sprite() :
		Texture(),
		horizScale(1.f),
		vertScale(1.f),
		theta(0.f),
		uX(1, 0),
		uY(0, 1),
		rotation(identityMatrix2)
	{ 
	}

	/** Texture constructor
	  **/
	Sprite(const Texture& tex) :
		Texture(tex),
		horizScale(1.f),
		vertScale(1.f),
		theta(0.f),
		uX(1, 0),
		uY(0, 1),
		rotation(identityMatrix2)
	{
		if(nContext == 0) // Convert the image to an OpenGL texture, if not converted already
			getTexture(); 
	}

	/** Filename constructor
	  * @param fname Path to input image file (png, bmp, etc)
	  * @param name The name of the sprite
	  **/
	Sprite(const std::string& fname, const std::string& name = "") :
		Texture(fname, name),
		horizScale(1.f),
		vertScale(1.f),
		theta(0.f),
		uX(1, 0),
		uY(0, 1),
		rotation(identityMatrix2)
	{
		getTexture(); // Convert the image to an OpenGL texture
	}

	~Sprite() { }

	/** Set the scaling factor of the horizontal and vertical axes of the sprite relative to its current scale
	  * @note The original aspect ratio of the sprite is preserved
	  * @param scale Scaling factor of the horizontal and vertical axes
	  **/
	void scale(const float& scale);

	/** Set the scaling factor of the horizontal and vertical axes of the sprite
	  * @note The original aspect ratio of the sprite is preserved
	  * @param scale Scaling factor of the horizontal and vertical axes
	  **/
	void setScale(const float& scale);

	/** Set the scaling factor of the horizontal axis of the sprite
	  * @param scale Scaling factor of the horizontal axis
	  **/
	void setHorizontalScale(const float& scale);

	/** Set the scaling factor of the vertical axis of the sprite
	  * @param scale Scaling factor of the vertical axis
	  **/
	void setVerticalScale(const float &scale);

	/** Set the rotation angle of the sprite relative to its current rotation
	  * @param angle Angle of sprite with respect to the horizontal axis (radians)
	  **/
	void rotate(const float& angle);

	/** Set the rotation angle of the sprite
	  * @param angle Angle of sprite with respect to the horizontal axis (radians)
	  **/
	void setRotation(const float& angle);

	/** Flip the horizontal axis of the sprite
	  * @note The rotation of the sprite is unaffected by the flip
	  **/
	void flipHorizontal();

	/** Flip the vertical axis of the sprite
	  * @note The rotation of the sprite is unaffected by the flip
	  **/
	void flipVertical();

	/** Draw the sprite, with position relative to its upper left corner 
	  * @note Horizontal and vertical mirroring will affect 
	  * @param x Horizontal coordinate of the upper left corner of the sprite
	  * @param y Vertical coordinate of the upper left corner of the sprite
	  **/
	void drawCorner(const int& x, const int& y);

	/** Draw the sprite, with position relative to its center
	  * @param x Horizontal coordinate of the center of the sprite
	  * @param y Vertical coordinate of the center of the sprite
	  **/
	void draw(const int& x, const int& y);

private:
	float horizScale; ///< Scaling factor along the horizontal axis of the sprite
	float vertScale; ///< Scaling factor along the vertical axis of the sprite

	float theta; ///< Current angle of rotation of the sprite (radians)

	Vector2 uX; ///< Psuedo-unit vector pointing from the center of the sprite to its right side
	Vector2 uY; ///< Psuedo-unit vector pointing from the center of the sprite to its top side

	Matrix2 rotation; ///< Two dimensional rotation vector for the current rotation of the sprite

	/** Create an OpenGL vertex from an input 2d vector
	  **/
	void getVertex(const Vector2& vec);

	/** Update the local horizontal and vertical axes of the sprite
	  **/
	void update();
};

#endif