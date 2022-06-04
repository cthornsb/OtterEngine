#include "OTTSprite.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

void ott::Sprite::Scale(const float& scale) {
	this->horizScale *= scale;
	this->vertScale *= scale;
	this->Update();
}

void ott::Sprite::SetScale(const float& scale) {
	this->horizScale = scale;
	this->vertScale = scale;
	this->Update();
}

void ott::Sprite::SetHorizontalScale(const float& scale) {
	this->horizScale = scale;
	this->Update();
}

void ott::Sprite::SetVerticalScale(const float& scale) {
	this->vertScale = scale;
	this->Update();
}

void ott::Sprite::Rotate(const float& angle) {
	this->rotation.SetRotation(theta += angle);
	this->Update();
}

void ott::Sprite::SetRotation(const float& angle) {
	this->rotation.SetRotation(theta = angle);
	this->Update();
}

void ott::Sprite::FlipHorizontal() { 
	this->horizScale *= -1.f;
	this->Update();
}

void ott::Sprite::FlipVertical() {
	this->vertScale *= -1.f;
	this->Update();
}

void ott::Sprite::DrawCorner() {
	this->DrawCorner(center - uX - uY);
}

void ott::Sprite::DrawCorner(const float& x, const float& y) {
	this->DrawCorner(Vector2(x, y));
}

void ott::Sprite::DrawCorner(const Vector2& offset) {
	this->OnUserBindTexture(); // Bind the texture
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
	this->OnUserUnbindTexture(); 
}

void ott::Sprite::Draw() {
	this->Draw(center);
}

void ott::Sprite::Draw(const float& x, const float& y) {
	this->Draw(Vector2(x, y));
}

void ott::Sprite::Draw(const Vector2& offset) {
	this->OnUserBindTexture(); // Bind the texture
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
	this->OnUserUnbindTexture(); 
}

void ott::Sprite::Update() {
	this->uX = rotation.Transform(Vector2(nWidth * horizScale / 2, 0));
	this->uY = rotation.Transform(Vector2(0, nHeight * vertScale / 2));
}

void ott::Sprite::OnUserBindTexture() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, nContext);
}

void ott::Sprite::OnUserUnbindTexture() {
	glDisable(GL_TEXTURE_2D);
}
