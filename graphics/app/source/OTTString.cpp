#include "OTTString.hpp"
#include "OTTCharacterMap.hpp"

bool ott::String::Alive() const {
	return (fLifetime < 0.f || fLifetime > fDeltaTime);
}

void ott::String::SetPosition(const int& x, const int& y) {
	nPosX = x;
	nPosY = y;
}

void ott::String::SetLifetime(const float& sustain, const float& decay/* = 0.f*/) {
	fTimeA = sustain;
	fTimeB = decay;
	fLifetime = (fTimeA > 0.f ? fTimeA + fTimeB : -1.f);
}

void ott::String::SetText(const std::string& str) {
	sBody = str;
	this->UpdateStringImage();
}

void ott::String::SetCharacterMap(CharacterMap* map) {
	cmap = map;
	this->UpdateStringImage();
}

bool ott::String::Print(ImageBuffer* imgbuff) {
	if (this->Alive() == false)
		return false;
	if (fLifetime > 0.f && fDeltaTime > fTimeA) {
		imgbuff->BlendSubImage(
			nPosX,
			nPosY,
			&buffer,
			1.f - (1.f / fTimeB) * (fDeltaTime - fTimeA) // Alpha between 1 and 0
		);
	}
	else {
		imgbuff->BlendSubImage(
			nPosX,
			nPosY,
			&buffer
		);
	}
	return true;
}

void ott::String::Update(const float& fWallTime) {
	fDeltaTime = fWallTime - fStartTime;
}

void ott::String::Reset(const float& fWallTime) {
	fStartTime = fWallTime;
	fDeltaTime = 0.f;
}

void ott::String::UpdateStringImage() {
	if (this->sBody == "") {
		return;
	}
	buffer.Resize(sBody.length() * cmap->CharacterWidth(), cmap->CharacterHeight(), 4);
	cmap->DrawString(sBody, 0, 0, &buffer);
}

void ott::StringManager::Add(const std::string& str, const int& x, const int& y, const float& lifetime, const float& decay/* = 0.f*/) {
	String* strptr = new String((float)timer.Stop());
	strptr->SetPosition(x, y);
	strptr->SetLifetime(lifetime, decay);
	strptr->SetCharacterMap(cmap);
	strptr->SetText(str);
	strings.push_back(std::unique_ptr<String>(strptr));
}

void ott::StringManager::Update(ImageBuffer* buffer, const float& fWallTime) {
	for (auto str = strings.begin(); str != strings.end(); str++) { // Update string lifetimes
		(*str)->Update(fWallTime);
	}
	if (bAutoRemove) {
		this->ClearInactive();
	}
	for (auto str = strings.begin(); str != strings.end(); str++) { // Print remaining strings
		(*str)->Print(buffer);
	}
}

void ott::StringManager::Update(ImageBuffer* buffer) {
	this->Update(buffer, (float)timer.Stop());
}

void ott::StringManager::Reset() {
	float fWallTime = (float)timer.Stop();
	for (auto str = strings.begin(); str != strings.end(); str++) { // Update string lifetimes
		(*str)->Reset(fWallTime);
	}
}

void ott::StringManager::Clear() {
	strings.clear();
}

void ott::StringManager::ClearInactive() {
	// erase_remove_if()
	strings.erase(
		std::remove_if(
			strings.begin(),
			strings.end(),
			[](const std::unique_ptr<String>& ptr) {
				return !ptr->Alive();
			}
		),
		strings.end()
	);
}
