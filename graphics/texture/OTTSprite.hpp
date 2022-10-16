#ifndef GRAPHICS_OTT_SPRITE_HPP
#define GRAPHICS_OTT_SPRITE_HPP

#include "OTTTexture.hpp"

#include <math/OTTMatrix.hpp>
#include <math/OTTVector.hpp>
#include <math/OTTWrappedValue.hpp>

#include <cstdint>
#include <vector>

namespace ott {

class Shader;

class Sprite : public Texture {
public:
	/** Default constructor
	  */
	Sprite(uint32_t context = 0) :
		Texture(),
		vertexVBO(0),
		rawOffsets(),
		theta(0.f, 0.f, 6.28318f),
		center(0.f, 0.f),
		scale(1.f, 1.f),
		uX(1.f, 0.f),
		uY(0.f, 1.f),
		rotation(constants::kIdentityMatrix2),
		shader(nullptr)
	{ 
		nContext = context;
	}

	/** Filename constructor
	  * @param fname Path to input image file (png, bmp, etc)
	  * @param name The name of the sprite
	  */
	Sprite(const std::string& fname, const std::string& name = "") :
		Texture(fname, name),
		vertexVBO(0),
		rawOffsets(),
		theta(0.f, 0.f, 6.28318f),
		center(0.f, 0.f),
		scale(1.f, 1.f),
		uX(1.f, 0.f),
		uY(0.f, 1.f),
		rotation(constants::kIdentityMatrix2),
		shader(nullptr)
	{
		this->GetTexture(); // Convert the image to an OpenGL texture
	}

	/** Destructor
	  */
	~Sprite();

	/** Set the scaling factor of the horizontal and vertical axes of the sprite relative to its current scale
	  * @note The original aspect ratio of the sprite is preserved
	  * @param factor Scaling factor of the horizontal and vertical axes
	  */
	void Scale(const float& factor);

	/** Set the scaling factor of the horizontal and vertical axes of the sprite
	  * @note The original aspect ratio of the sprite is preserved
	  * @param factor Scaling factor of the horizontal and vertical axes
	  */
	void SetScale(const float& factor);

	/** Set the scaling factor of the horizontal and vertical axes of the sprite
	  * @param x Scaling factor of the horizontal axis
	  * @param y Scaling factor of the vertical axis
	  */
	void SetScale(const float& x, const float& y);

	/** Set the position of the sprite on the screen
	  */
	void SetPosition(const float& x, const float& y){
		center = Vector2(x, y);
	}

	/** Set the position of the sprite on the screen
	  */
	void SetPosition(const Vector2& pos){
		center = pos;
	}

	/** Set the rotation angle of the sprite relative to its current rotation
	  * @param angle Angle of sprite with respect to the horizontal axis (radians)
	  */
	void Rotate(const float& angle);

	/** Set the rotation angle of the sprite
	  * @param angle Angle of sprite with respect to the horizontal axis (radians)
	  */
	void SetRotation(const float& angle);

	/** Flip the horizontal axis of the sprite
	  * @note The rotation of the sprite is unaffected by the flip
	  */
	void FlipHorizontal();

	/** Flip the vertical axis of the sprite
	  * @note The rotation of the sprite is unaffected by the flip
	  */
	void FlipVertical();

	/** Set the shader to use for rendering.
	  * @param shdr Pointer to the shader to use for rendering this sprite
	  */
	void SetShader(Shader* shdr, bool reverse_winding = false) {
		this->shader = shdr;
		this->SetupGeometry(reverse_winding);
	}

	/** Draw the sprite at its current position relative to its center
	  */
	void Draw();

protected:

	uint32_t vertexVBO;

	std::vector<size_t> rawOffsets;

	WrappedValue theta; ///< Current angle of rotation of the sprite (radians)

	Vector2 center; ///< NDC position of center of sprite on screen (+/- Z is into or out of the screen)

	Vector2 scale; ///< Scaling factor along the horizontal and vertical axes of the sprite

	Vector2 uX; ///< Vector along sprite's local horizontal axis with length of half the width of sprite
	
	Vector2 uY; ///< Vector along sprite's local vertical axis with length of half the height of sprite

	Matrix2 rotation; ///< Two dimensional rotation vector for the current rotation of the sprite

	Shader* shader; ///< Pointer to an OpenGL shader to use for rendering
	
	void UpdateUnitVectors();

	void SetupGeometry(bool reverse_winding);
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_SPRITE_HPP
