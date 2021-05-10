#ifndef OTT_SPRITE_HPP
#define OTT_SPRITE_HPP

#include "OTTTexture.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include "WrappedValue.hpp"

class OTTSprite : public OTTTexture {
public:
	/** Default constructor
	  **/
	OTTSprite() :
		OTTTexture(),
		horizScale(1.f),
		vertScale(1.f),
		theta(0.f, 0.f, 6.28318f),
		center(0.f, 0.f),
		uX(1.f, 0.f),
		uY(0.f, 1.f),
		rotation(identityMatrix2)
	{ 
	}

	/** Filename constructor
	  * @param fname Path to input image file (png, bmp, etc)
	  * @param name The name of the sprite
	  **/
	OTTSprite(const std::string& fname, const std::string& name = "") :
		OTTTexture(fname, name),
		horizScale(1.f),
		vertScale(1.f),
		theta(0.f, 0.f, 6.28318f),
		center(0.f, 0.f),
		uX(1.f, 0.f),
		uY(0.f, 1.f),
		rotation(identityMatrix2)
	{
		getTexture(); // Convert the image to an OpenGL texture
	}

	/** Destructor
	  */
	~OTTSprite() { 
	}

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

	/** Set the position of the sprite on the screen
	  */
	void setPosition(const float& x, const float& y){
		center = Vector2(x, y);
	}

	/** Set the position of the sprite on the screen
	  */
	void setPosition(const Vector2& pos){
		center = pos;
	}

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

	/** Draw the sprite at its current position relative to its upper left corner 
	  * @note Horizontal and vertical mirroring will affect 
	  **/
	void drawCorner();

	/** Draw the sprite, with position relative to its upper left corner 
	  * @note Horizontal and vertical mirroring will affect 
	  * @param x Horizontal coordinate of the upper left corner of the sprite
	  * @param y Vertical coordinate of the upper left corner of the sprite
	  **/
	void drawCorner(const float& x, const float& y);

	/** Draw the sprite, with position relative to its upper left corner 
	  * @note Horizontal and vertical mirroring will affect 
	  * @param offset Pixel coordinate of the upper left corner of the sprite
	  **/
	void drawCorner(const Vector2& offset);

	/** Draw the sprite at its current position relative to its center
	  **/
	void draw();

	/** Draw the sprite, with position relative to its center
	  * @param x Horizontal coordinate of the center of the sprite
	  * @param y Vertical coordinate of the center of the sprite
	  **/
	void draw(const float& x, const float& y);

	/** Draw the sprite, with position relative to its center
	  * @param offset Pixel coordinate of the center of the sprite
	  **/
	void draw(const Vector2& offset);

protected:
	float horizScale; ///< Scaling factor along the horizontal axis of the sprite
	
	float vertScale; ///< Scaling factor along the vertical axis of the sprite

	WrappedValue theta; ///< Current angle of rotation of the sprite (radians)

	Vector2 center; ///< Position of center of sprite on screen (in pixels)

	Vector2 uX; ///< Vector along sprite's local horizontal axis with length of half the width of sprite
	
	Vector2 uY; ///< Vector along sprite's local vertical axis with length of half the height of sprite

	Matrix2 rotation; ///< Two dimensional rotation vector for the current rotation of the sprite

	/** Update the local horizontal and vertical axes of the sprite
	  **/
	void update();
	
	virtual void onUserBindTexture();
	
	virtual void onUserUnbindTexture();
};

#endif
