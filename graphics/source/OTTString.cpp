#include "OTTString.hpp"
#include "OTTCharacterMap.hpp"

bool OTTString::isAlive() const {
	return (fLifetime < 0.f || fLifetime > fDeltaTime);
}

void OTTString::setPosition(const int& x, const int& y) {
	nPosX = x;
	nPosY = y;
}

void OTTString::setLifetime(const float& sustain, const float& decay/* = 0.f*/) {
	fTimeA = sustain;
	fTimeB = decay;
	fLifetime = (fTimeA > 0.f ? fTimeA + fTimeB : -1.f);
}

void OTTString::setText(const std::string& str) {
	sBody = str;
	updateStringImage();
}

void OTTString::setCharacterMap(OTTCharacterMap* map) {
	cmap = map;
	updateStringImage();
}

bool OTTString::print(OTTImageBuffer* imgbuff) {
	if (!isAlive())
		return false;
	if (fLifetime > 0.f && fDeltaTime > fTimeA) {
		imgbuff->blendSubImage(
			nPosX,
			nPosY,
			&buffer,
			1.f - (1.f / fTimeB) * (fDeltaTime - fTimeA) // Alpha between 1 and 0
		);
	}
	else {
		imgbuff->blendSubImage(
			nPosX,
			nPosY,
			&buffer
		);
	}
	return true;
}

void OTTString::update(const float& fWallTime) {
	fDeltaTime = fWallTime - fStartTime;
}

void OTTString::reset(const float& fWallTime) {
	fStartTime = fWallTime;
	fDeltaTime = 0.f;
}

void OTTString::updateStringImage() {
	buffer.resize(sBody.length() * cmap->getCharacterWidth(), cmap->getCharacterHeight(), 4);
	cmap->drawString(sBody, 0, 0, &buffer);
}

void OTTStringManager::add(const std::string& str, const int& x, const int& y, const float& lifetime, const float& decay/* = 0.f*/) {
	OTTString* strptr = new OTTString((float)timer.stop());
	strptr->setPosition(x, y);
	strptr->setLifetime(lifetime, decay);
	strptr->setCharacterMap(cmap);
	strptr->setText(str);
	strings.push_back(std::unique_ptr<OTTString>(strptr));
}

void OTTStringManager::update(OTTImageBuffer* buffer, const float& fWallTime) {
	for (auto str = strings.begin(); str != strings.end(); str++) { // Update string lifetimes
		(*str)->update(fWallTime);
	}
	if (bAutoRemove) {
		clearInactive();
	}
	for (auto str = strings.begin(); str != strings.end(); str++) { // Print remaining strings
		(*str)->print(buffer);
	}
}

void OTTStringManager::update(OTTImageBuffer* buffer) {
	update(buffer, (float)timer.stop());
}

void OTTStringManager::reset() {
	float fWallTime = (float)timer.stop();
	for (auto str = strings.begin(); str != strings.end(); str++) { // Update string lifetimes
		(*str)->reset(fWallTime);
	}
}

void OTTStringManager::clear() {
	strings.clear();
}

void OTTStringManager::clearInactive() {
	// erase_remove_if()
	strings.erase(
		std::remove_if(
			strings.begin(),
			strings.end(),
			[](const std::unique_ptr<OTTString>& ptr) {
				return !ptr->isAlive();
			}
		),
		strings.end()
	);
}
