#ifndef OTT_SPRITE_SET_HPP
#define OTT_SPRITE_SET_HPP

#include <string>
#include <vector>

#include "OTTTexture.hpp"
#include "OTTSprite.hpp"

class OTTSpriteSet : public OTTSprite {
public:
	/** Default constructor
	  */
	OTTSpriteSet() :
		OTTSprite(),
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
	OTTSpriteSet(const std::string& fname, const int& px, const int& py) :
		OTTSprite(),
		sName(),
		dFramePeriod(1 / 60.0),
		dAnimationLength(0),
		dTotalTime(0),
		currentSprite(),
		spriteList()
	{
		addSprites(fname, px, py);
	}
	
	/** Copy constructor
	  */
	OTTSpriteSet(const OTTSpriteSet&) = delete;
	
	/** Assignment operator
	  */
	OTTSpriteSet& operator = (const OTTSpriteSet&) = delete;
	
	/** Destructor
	  */
	~OTTSpriteSet(){ 
	}

	/**
	  */
	OTTSpriteSet& operator [] (const size_t& index);
	
	/** Set the rate at which sprite frames are advanced (in frames per second)
	  */
	void setFramerate(const double& fps);
	
	/** Revert to the previous frame in the sequence
	  */
	void prev();
	
	/** Advance to the next frame in the sequence
	  */
	void next();
	
	/** Reset sprite animation to the first frame
	  */
	void reset();
	
	/** Load a new sprite from an image file
	  * @return True if sprite image loaded successfully
	  */
	bool addSprite(const std::string& fname);
	
	/** Load a new sprite from an image file
	  * @return True if sprite image loaded successfully
	  */
	bool addSprite(const std::string& fname, const std::string& name);
	
	/** Copy a sprite from a region in an existing image buffer
	  */
	bool addSprite(OTTImageBuffer* buffer, const int& x0, const int& y0, const int& px, const int& py);
	
	/** Load multiple sprites form a single image file, with each sprite being px pixels wide and py pixels tall
	  * @return True if sprite image loaded successfully
	  */
	bool addSprites(const std::string& fname, const int& px, const int& py);

	/** Load multiple sprites form a single image file, with each sprite being px pixels wide and py pixels tall
	  * @return True if sprite image loaded successfully
	  */
	bool addSprites(const std::string& fname, const std::string& name, const int& px, const int& py);

	/** Copy a number of sprites from a region in an existing image buffer
	  */
	bool addSprites(OTTImageBuffer* buffer, const int& x0, const int& y0, const int& px, const int& py, const int& cols, const int& rows);

	/** Update the current sprite animation time, advancing to the next frame if needed
	  */
	void update(const double& dTime);
	
protected:
	std::string sName; ///< Name of sprite set
	
	double dFramePeriod; ///< Time between successive frame images

	double dAnimationLength; ///< Total time for all 

	double dTotalTime; ///< Current time since the beginning of sprite animation loop
	
	std::vector<unsigned int>::iterator currentSprite; ///< Iterator to currently selected sprite image

	std::vector<unsigned int> spriteList; ///< Vector of all loaded sprites
};

#endif // ifndef OTT_SPRITE_SET_HPP
