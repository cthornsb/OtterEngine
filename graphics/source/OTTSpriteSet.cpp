#include <iostream>
#include <algorithm> // copy
#include <memory> // unique_ptr

#include "OTTSpriteSet.hpp"

void OTTSpriteSet::setFramerate(const double& fps){
	dFramePeriod = 1 / fps;
	dAnimationLength = dFramePeriod * spriteList.size();
}

void OTTSpriteSet::prev(){
	if(currentSprite != spriteList.begin())
		currentSprite--;
	else
		currentSprite = spriteList.end() - 1;
	nContext = (*currentSprite);
}

void OTTSpriteSet::next(){
	if(++currentSprite == spriteList.end())
		currentSprite = spriteList.begin();
	nContext = (*currentSprite);
}

void OTTSpriteSet::reset(){
	dTotalTime = 0;
	currentSprite = spriteList.begin();
	nContext = (*currentSprite);
}

bool OTTSpriteSet::addSprite(const std::string& fname){
	std::unique_ptr<OTTTexture> sprite(new OTTTexture());
	
	// Set texture name
	if(!sName.empty())
		sprite->setName(sName);
	
	// Read input image file
	if(!sprite->read(fname))
		return false;

	// Transfer image data to GPU
	sprite->getTexture();
	
	// Add new sprite to the list of sprites
	spriteList.push_back(sprite->getContext());

	// Reset to the first frame in the sequence
	reset();

	// Update time of total animation length
	dAnimationLength = dFramePeriod * spriteList.size();
	
	return true;
}

bool OTTSpriteSet::addSprite(const std::string& fname, const std::string& name){
	sName = name;
	return addSprite(fname);
}

bool OTTSpriteSet::addSprite(OTTImageBuffer* buffer, const int& x0, const int& y0, const int& px, const int& py){
	return addSprites(buffer, x0, y0, px, py, 1, 1);
}

bool OTTSpriteSet::addSprites(const std::string& fname, const int& px, const int& py){
	std::unique_ptr<OTTTexture> sprite(new OTTTexture());

	// Read input image file
	if(!sprite->read(fname))
		return false;

	// Iterate over all sub-images
	return addSprites(sprite.get(), 0, 0, px, py, sprite->getWidth() / px, sprite->getHeight() / py);
}

bool OTTSpriteSet::addSprites(const std::string& fname, const std::string& name, const int& px, const int& py){
	sName = name;
	return addSprites(fname, px, py);
}

bool OTTSpriteSet::addSprites(OTTImageBuffer* buffer, const int& x0, const int& y0, const int& px, const int& py, const int& cols, const int& rows){
	int subImageWidth = x0 + px * cols;
	if(subImageWidth >= buffer->getWidth()){
		std::cout << " [OTTSpriteSet] Warning! Requested sub-image width (" << subImageWidth << ") exceeds input image width of " << buffer->getWidth() << " pixels." << std::endl;
	}

	int subImageHeight = y0 + py * rows;
	if(subImageHeight >= buffer->getHeight()){
		std::cout << " [OTTSpriteSet] Warning! Requested sub-image height (" << subImageHeight << ") exceeds input image height of " << buffer->getHeight() << " pixels." << std::endl;
	}

	// Set sprite unit vectors
	nWidth = px;
	nHeight = py;
	uX = Vector2(px / 2, 0);
	uY = Vector2(0, py / 2);

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
				if (pixelY + i >= buffer->getHeight()) // Invalid image coordinates
					break;
				// Extract a row of pixels
				unsigned char* start = buffer->get(pixelX, pixelY + i);
				if(!start)
					break;
				std::copy(start, start + px * 4, &subimage[(size_t)((i * px) * 4)]);
			}
			
			// Generate an OpenGL texture from the pixel data
			spriteList.push_back(OTTTexture::generateTextureRGBA(px, py, subimage.get(), false));
			
			pixelX += px;
		}
		pixelY += py;
	}
	
	//if(verboseMode)
	//	std::cout << " [OTTSpriteSet] Added " << spriteList.size() << " frames to sprite." << std::endl;

	// Reset to the first frame in the sequence
	reset();
	
	// Update time of total animation length
	dAnimationLength = dFramePeriod * spriteList.size();

	return true;
}

void OTTSpriteSet::update(const double& dTime){
	dTotalTime += dTime;
	if(dTotalTime >= dAnimationLength){ // Reset animation to first frame
		dTotalTime -= dAnimationLength;
	}
	int currentFrame = (int)(dTotalTime / dFramePeriod);
	currentSprite = spriteList.begin() + currentFrame;
	nContext = (*currentSprite);
}

