#include <GL/freeglut.h>

#include "Sprite.hpp"

void Sprite::scale(const float& scale) {
	horizScale *= scale;
	vertScale *= scale;
	update();
}

void Sprite::setScale(const float& scale) {
	horizScale = scale;
	vertScale = scale;
	update();
}

void Sprite::setHorizontalScale(const float& scale) {
	horizScale = scale;
	update();
}

void Sprite::setVerticalScale(const float& scale) {
	vertScale = scale;
	update();
}

void Sprite::rotate(const float& angle) {
	rotation.setRotation(theta += angle);
	update();
}

void Sprite::setRotation(const float& angle) {
	rotation.setRotation(theta = angle);
	update();
}

void Sprite::flipHorizontal() { 
	horizScale *= -1; 
	update();
}

void Sprite::flipVertical() {
	vertScale *= -1; 
	update();
}

void Sprite::drawCorner(const int& x, const int& y) {
	vector2 offset((float)x, (float)y);
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

void Sprite::draw(const int& x, const int& y) {
	vector2 offset((float)x, (float)y);
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

void Sprite::getVertex(const vector2& vec) {
	glVertex2i((GLint)vec.x, (GLint)vec.y);
}

void Sprite::update() {
	uX = vector2(nWidth * horizScale / 2, 0);
	uY = vector2(0, nHeight * vertScale / 2);
	rotation.transformInPlace(uX);
	rotation.transformInPlace(uY);
}