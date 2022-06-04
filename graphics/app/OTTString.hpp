#ifndef GRAPHICS_OTT_STRING_HPP
#define GRAPHICS_OTT_STRING_HPP

#include <core/OTTHighResTimer.hpp>
#include <graphics/core/OTTImageBuffer.hpp>

#include <string>
#include <vector>
#include <memory>

namespace ott {

class CharacterMap;

class String {
public:
	String() = delete;

	String(const float& start) :
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

	String(const String&) = delete;

	String& operator = (const String&) = delete;

	bool Alive() const;

	void SetPosition(const int& x, const int& y);

	void SetLifetime(const float& sustain, const float& decay = 0.f);

	void SetText(const std::string& str);

	void SetCharacterMap(CharacterMap* map);

	bool Print(ImageBuffer* imgbuff);

	void Update(const float& fWallTime);

	/** Reset lifetime of string to its original value
	  */
	void Reset(const float& fWallTime);

private:
	int nPosX;

	int nPosY;

	float fTimeA; ///< 

	float fTimeB; ///< Decay time of string opacity

	float fLifetime; ///< Total lifetime of string

	float fStartTime; ///< Time that string was created

	float fDeltaTime; ///< Time elapsed since string was created

	ImageBuffer buffer;

	CharacterMap* cmap;

	std::string sBody;

	/** Render string to graphical text using the character map
	  */
	void UpdateStringImage();
};

class StringManager {
public:
	StringManager() :
		bAutoRemove(true),
		timer(),
		strings(),
		cmap()
	{
	}

	void SetCharacterMap(CharacterMap* map) {
		cmap = map; //std::make_shared<OTTCharacterMap>(map);
	}

	void Add(const std::string& str, const int& x, const int& y, const float& lifetime, const float& decay = 0.f);

	/** Update the remaining lifetime of all strings using an external time
	  */
	void Update(ImageBuffer* buffer, const float& fWallTime);

	/** Update the remaining lifetime of all strings using the built in timer
	  */
	void Update(ImageBuffer* buffer);

	/** Reset lifetime of all strings
	  */
	void Reset();

	/** Clear all strings
	  */
	void Clear();

	/** Clear all currently inactive strings
	  */
	void ClearInactive();

private:
	bool bAutoRemove;

	HighResTimer timer;

	std::vector<std::unique_ptr<String> > strings;

	CharacterMap* cmap;
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_STRING_HPP
