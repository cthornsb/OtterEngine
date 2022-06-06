#ifndef GRAPHICS_OTT_SPRITE_SET_HPP
#define GRAPHICS_OTT_SPRITE_SET_HPP

#include "OTTTexture.hpp"
#include "OTTSprite.hpp"

#include <string>
#include <vector>

namespace ott {

class SpriteSet : public Sprite {
public:
	/** Default constructor
	  */
	SpriteSet() :
		Sprite(),
		sName(),
		dFramePeriod(1 / 60.0),
		dAnimationLength(0),
		dTotalTime(0),
		currentSprite(),
		spriteList()
	{
	}
	
	/** Sprite sheet constructor
	  * @param px Horizontal size of each sprite in sprite sheet (in pixels)
	  * @param py Vertical size of each sprite in sprite sheet (in pixels)
	  */
	SpriteSet(const std::string& fname, const int32_t& px, const int32_t& py) :
		Sprite(),
		sName(),
		dFramePeriod(1 / 60.0),
		dAnimationLength(0),
		dTotalTime(0),
		currentSprite(),
		spriteList()
	{
		this->AddSprites(fname, px, py);
	}
	
	/** Copy constructor
	  */
	SpriteSet(const SpriteSet&) = delete;
	
	/** Destructor
	  */
	~SpriteSet(){ 
	}

	/** Assignment operator
	  */
	SpriteSet& operator = (const SpriteSet&) = delete;

	/** Set the current frame of animation
	  */
	SpriteSet& operator [] (const size_t& index);
	
	/** Set the rate at which sprite frames are advanced (in frames per second)
	  */
	void SetFramerate(const double& fps);
	
	/** Revert to the previous frame in the sequence
	  */
	void Prev();
	
	/** Advance to the next frame in the sequence
	  */
	void Next();
	
	/** Reset sprite animation to the first frame
	  */
	void Reset();
	
	/** Load a new sprite from an image file
	  * @return True if sprite image loaded successfully
	  */
	bool AddSprite(const std::string& fname);
	
	/** Load a new sprite from an image file
	  * @return True if sprite image loaded successfully
	  */
	bool AddSprite(const std::string& fname, const std::string& name);
	
	/** Copy a sprite from a region in an existing image buffer
	  */
	bool AddSprite(ImageBuffer* buffer, const int32_t& x0, const int32_t& y0, const int32_t& px, const int32_t& py);
	
	/** Load multiple sprites form a single image file, with each sprite being px pixels wide and py pixels tall
	  * @return True if sprite image loaded successfully
	  */
	bool AddSprites(const std::string& fname, const int32_t& px, const int32_t& py);

	/** Load multiple sprites form a single image file, with each sprite being px pixels wide and py pixels tall
	  * @return True if sprite image loaded successfully
	  */
	bool AddSprites(const std::string& fname, const std::string& name, const int32_t& px, const int32_t& py);

	/** Copy a number of sprites from a region in an existing image buffer
	  */
	bool AddSprites(ImageBuffer* buffer, const int32_t& x0, const int32_t& y0, const int32_t& px, const int32_t& py, const int32_t& cols, const int32_t& rows);

	/** Update the current sprite animation time, advancing to the next frame if needed
	  */
	void Update(const double& dTime);
	
protected:
	std::string sName; ///< Name of sprite set
	
	double dFramePeriod; ///< Time between successive frame images

	double dAnimationLength; ///< Total time for all 

	double dTotalTime; ///< Current time since the beginning of sprite animation loop
	
	std::vector<uint32_t>::iterator currentSprite; ///< Iterator to currently selected sprite image

	std::vector<uint32_t> spriteList; ///< Vector of all loaded sprites
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_SPRITE_SET_HPP
