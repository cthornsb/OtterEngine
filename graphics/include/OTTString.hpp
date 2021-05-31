#ifndef OTT_STRING_HPP
#define OTT_STRING_HPP

#include <string>
#include <vector>
#include <memory>

#include "OTTImageBuffer.hpp"
#include "OTTHighResTimer.hpp"

class OTTCharacterMap;

class OTTString {
public:
	OTTString() = delete;

	OTTString(const float& start) :
		nPosX(0),
		nPosY(0),
		fTimeA(-1.f),
		fTimeB(-1.f),
		fLifetime(-1.f),
		fStartTime(start),
		fDeltaTime(0.f),
		buffer(),
		cmap(0x0),
		sBody()
	{
	}

	OTTString(const OTTString&) = delete;

	OTTString& operator = (const OTTString&) = delete;

	bool isAlive() const;

	void setPosition(const int& x, const int& y);

	void setLifetime(const float& sustain, const float& decay = 0.f);

	void setText(const std::string& str);

	void setCharacterMap(OTTCharacterMap* map);

	bool print(OTTImageBuffer* imgbuff);

	void update(const float& fWallTime);

	/** Reset lifetime of string to its original value
	  */
	void reset(const float& fWallTime);

private:
	int nPosX;

	int nPosY;

	float fTimeA; ///< 

	float fTimeB; ///< Decay time of string opacity

	float fLifetime; ///< Total lifetime of string

	float fStartTime; ///< Time that string was created

	float fDeltaTime; ///< Time elapsed since string was created

	OTTImageBuffer buffer;

	OTTCharacterMap* cmap;

	std::string sBody;

	/** Render string to graphical text using the character map
	  */
	void updateStringImage();
};

class OTTStringManager {
public:
	OTTStringManager() :
		bAutoRemove(true),
		timer(),
		strings(),
		cmap()
	{
	}

	void setCharacterMap(OTTCharacterMap* map) {
		cmap = map; //std::make_shared<OTTCharacterMap>(map);
	}

	void add(const std::string& str, const int& x, const int& y, const float& lifetime, const float& decay = 0.f);

	/** Update the remaining lifetime of all strings using an external time
	  */
	void update(OTTImageBuffer* buffer, const float& fWallTime);

	/** Update the remaining lifetime of all strings using the built in timer
	  */
	void update(OTTImageBuffer* buffer);

	/** Reset lifetime of all strings
	  */
	void reset();

	/** Clear all strings
	  */
	void clear();

	/** Clear all currently inactive strings
	  */
	void clearInactive();

private:
	bool bAutoRemove;

	OTTHighResTimer timer;

	std::vector<std::unique_ptr<OTTString> > strings;

	OTTCharacterMap* cmap; //std::shared_ptr<OTTCharacterMap> cmap;
};

#endif // ifndef OTT_STRING_HPP