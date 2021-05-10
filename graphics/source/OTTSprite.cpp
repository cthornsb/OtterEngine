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
	horizScale *= -1.f; 
	update();
}

void OTTSprite::flipVertical() {
	vertScale *= -1.f; 
	update();
}

void OTTSprite::drawCorner() {
	drawCorner(center - uX - uY);
}

void OTTSprite::drawCorner(const float& x, const float& y) {
	drawCorner(Vector2(x, y));
}

void OTTSprite::drawCorner(const Vector2& offset) {
	this->onUserBindTexture(); // Bind the texture
	glBegin(GL_QUADS);
	auto setVertex = [](const float& tx, const float& ty, const Vector2& vec) {
		glTexCoord2f(tx, ty);
		glVertex2f(vec[0], vec[1]);
	};
	setVertex(0.f, 0.f, offset);
	setVertex(1.f, 0.f, offset + uX * 2.f);
	setVertex(1.f, 1.f, offset + uX * 2.f + uY * 2.f);
	setVertex(0.f, 1.f, offset + uY * 2.f);
	glEnd();
	this->onUserUnbindTexture(); 
}

void OTTSprite::draw() {
	draw(center);
}

void OTTSprite::draw(const float& x, const float& y) {
	draw(Vector2(x, y));
}

void OTTSprite::draw(const Vector2& offset) {
	this->onUserBindTexture(); // Bind the texture
	glBegin(GL_QUADS);
	auto setVertex = [](const float& tx, const float& ty, const Vector2& vec) {
		glTexCoord2f(tx, ty);
		glVertex2f(vec[0], vec[1]);
	};
	setVertex(0.f, 0.f, offset - uX - uY);
	setVertex(1.f, 0.f, offset + uX - uY);
	setVertex(1.f, 1.f, offset + uX + uY);
	setVertex(0.f, 1.f, offset - uX + uY);
	glEnd();
	this->onUserUnbindTexture(); 
}

void OTTSprite::update() {
	uX = rotation.transform(Vector2(nWidth * horizScale / 2, 0));
	uY = rotation.transform(Vector2(0, nHeight * vertScale / 2));
}

void OTTSprite::onUserBindTexture() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, nContext);
}

void OTTSprite::onUserUnbindTexture() {
	glDisable(GL_TEXTURE_2D);
}
