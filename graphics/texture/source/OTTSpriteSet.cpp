#include "OTTSpriteSet.hpp"

#include <iostream>
#include <algorithm> // copy
#include <memory> // unique_ptr

ott::SpriteSet& ott::SpriteSet::operator [] (const size_t& index) {
	if (index < spriteList.size()) {
		currentSprite = spriteList.begin() + index;
	}
	return (*this);
}

void ott::SpriteSet::SetFramerate(const double& fps){
	dFramePeriod = 1 / fps;
	dAnimationLength = dFramePeriod * spriteList.size();
}

void ott::SpriteSet::Prev(){
	if (currentSprite != spriteList.begin()) {
		currentSprite--;
	}
	else {
		currentSprite = spriteList.end() - 1;
	}
	nContext = (*currentSprite);
}

void ott::SpriteSet::Next(){
	if (++currentSprite == spriteList.end()) {
		currentSprite = spriteList.begin();
	}
	nContext = (*currentSprite);
}

void ott::SpriteSet::Reset(){
	dTotalTime = 0;
	currentSprite = spriteList.begin();
	nContext = (*currentSprite);
}

bool ott::SpriteSet::AddSprite(const std::string& fname){
	std::unique_ptr<Texture> sprite(new Texture());
	
	// Set texture name
	if(!sName.empty())
		sprite->SetName(sName);
	
	// Read input image file
	if(!sprite->Read(fname))
		return false;

	// Transfer image data to GPU
	sprite->GetTexture();
	
	// Add new sprite to the list of sprites
	spriteList.push_back(sprite->Context());

	// Reset to the first frame in the sequence
	this->Reset();

	// Update time of total animation length
	dAnimationLength = dFramePeriod * spriteList.size();
	
	return true;
}

bool ott::SpriteSet::AddSprite(const std::string& fname, const std::string& name){
	sName = name;
	return this->AddSprite(fname);
}

bool ott::SpriteSet::AddSprite(ImageBuffer* buffer, const int& x0, const int& y0, const int& px, const int& py){
	return this->AddSprites(buffer, x0, y0, px, py, 1, 1);
}

bool ott::SpriteSet::AddSprites(const std::string& fname, const int& px, const int& py){
	std::unique_ptr<Texture> sprite(new Texture());

	// Read input image file
	if (!sprite->Read(fname)) {
		return false;
	}

	// Iterate over all sub-images
	return this->AddSprites(sprite.get(), 0, 0, px, py, sprite->Width() / px, sprite->Height() / py);
}

bool ott::SpriteSet::AddSprites(const std::string& fname, const std::string& name, const int& px, const int& py){
	sName = name;
	return this->AddSprites(fname, px, py);
}

bool ott::SpriteSet::AddSprites(ImageBuffer* buffer, const int& x0, const int& y0, const int& px, const int& py, const int& cols, const int& rows){
	int subImageWidth = x0 + px * cols;
	if(subImageWidth > buffer->Width()){
		std::cout << " [OTTSpriteSet] Warning! Requested sub-image width (" << subImageWidth << ") exceeds input image width of " << buffer->Width() << " pixels." << std::endl;
	}

	int subImageHeight = y0 + py * rows;
	if(subImageHeight > buffer->Height()){
		std::cout << " [OTTSpriteSet] Warning! Requested sub-image height (" << subImageHeight << ") exceeds input image height of " << buffer->Height() << " pixels." << std::endl;
	}

	// Set sprite unit vectors
	nWidth = px;
	nHeight = py;
	uX = Vector2(px / 2.f, 0);
	uY = Vector2(0, py / 2.f);

	// Pixel data storage array
	std::unique_ptr<unsigned char[]> subimage(new unsigned char[px * py * 4]);

	// Iterate over all sub-images
	int pixelX = x0;
	int pixelY = y0;
	for(int row = 0; row < rows; row++){
		pixelX = x0;
		for(int col = 0; col < cols; col++){
			// Extract pixel data
			for (unsigned short i = 0; i < py; i++) { // Over rows
				if (pixelY + i >= buffer->Height()) // Invalid image coordinates
					break;
				// Extract a row of pixels
				unsigned char* start = buffer->Get(pixelX, pixelY + i);
				if(!start)
					break;
				std::copy(start, start + px * 4, &subimage[(size_t)((i * px) * 4)]);
			}
			
			// Generate an OpenGL texture from the pixel data
			spriteList.push_back(Texture::GenerateTextureRGBA(px, py, subimage.get(), false));
			
			pixelX += px;
		}
		pixelY += py;
	}
	
	//if(verboseMode)
	//	std::cout << " [OTTSpriteSet] Added " << spriteList.size() << " frames to sprite." << std::endl;

	// Reset to the first frame in the sequence
	this->Reset();
	
	// Update time of total animation length
	dAnimationLength = dFramePeriod * spriteList.size();

	return true;
}

void ott::SpriteSet::Update(const double& dTime){
	dTotalTime += dTime;
	if(dTotalTime >= dAnimationLength){ // Reset animation to first frame
		dTotalTime -= dAnimationLength;
	}
	int currentFrame = (int)(dTotalTime / dFramePeriod);
	currentSprite = spriteList.begin() + currentFrame;
	nContext = (*currentSprite);
}

