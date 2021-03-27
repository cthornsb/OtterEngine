#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "OTTSprite.hpp"

void OTTSprite::scale(const float& scale) {
	horizScale *= scale;
	vertScale *= scale;
	update();
}

void OTTSprite::setScale(const float& scale) {
	horizScale = scale;
	vertScale = scale;
	update();
}

void OTTSprite::setHorizontalScale(const float& scale) {
	horizScale = scale;
	update();
}

void OTTSprite::setVerticalScale(const float& scale) {
	vertScale = scale;
	update();
}

void OTTSprite::rotate(const float& angle) {
	rotation.setRotation(theta += angle);
	update();
}

void OTTSprite::setRotation(const float& angle) {
	rotation.setRotation(theta = angle);
	update();
}

void OTTSprite::flipHorizontal() { 
	horizScale *= -1; 
	update();
}

void OTTSprite::flipVertical() {
	vertScale *= -1; 
	update();
}

void OTTSprite::drawCorner(const int& x, const int& y) {
	Vector2 offset((float)x, (float)y);
	glBindTexture(GL_TEXTURE_2D, nContext);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); getVertex(offset);
	glTexCoord2i(1, 0); getVertex(offset + uX * 2);
	glTexCoord2i(1, 1); getVertex(offset + uX * 2 + uY * 2);
	glTexCoord2i(0, 1); getVertex(offset + uY * 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OTTSprite::draw(const int& x, const int& y) {
	Vector2 offset((float)x, (float)y);
	glBindTexture(GL_TEXTURE_2D, nContext);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); getVertex(offset - uX - uY);
	glTexCoord2i(1, 0); getVertex(offset + uX - uY);
	glTexCoord2i(1, 1); getVertex(offset + uX + uY);
	glTexCoord2i(0, 1); getVertex(offset - uX + uY);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OTTSprite::getVertex(const Vector2& vec) {
	glVertex2i((GLint)vec[0], (GLint)vec[1]);
}

void OTTSprite::update() {
	uX = Vector2(nWidth * horizScale / 2, 0);
	uY = Vector2(0, nHeight * vertScale / 2);
	rotation.transformInPlace(uX);
	rotation.transformInPlace(uY);
}
