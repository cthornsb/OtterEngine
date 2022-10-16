#include "OTTSprite.hpp"

#include <graphics/core/OTTShader.hpp>
#include <graphics/core/OTTWindow.hpp>

ott::Sprite::~Sprite() {
	if (vertexVBO) {
		glDeleteBuffers(1, &vertexVBO);
	}
}

void ott::Sprite::Scale(const float& factor) {
	this->scale *= factor;
	this->UpdateUnitVectors();
}

void ott::Sprite::SetScale(const float& factor) {
	this->scale[0] = factor;
	this->scale[1] = factor;
	this->UpdateUnitVectors();
}

void ott::Sprite::SetScale(const float& x, const float& y) {
	this->scale[0] = x;
	this->scale[1] = y;
	this->UpdateUnitVectors();
}

void ott::Sprite::Rotate(const float& angle) {
	this->rotation.SetRotation(theta += angle);
	this->UpdateUnitVectors();
}

void ott::Sprite::SetRotation(const float& angle) {
	this->rotation.SetRotation(theta = angle);
	this->UpdateUnitVectors();
}

void ott::Sprite::FlipHorizontal() { 
	this->scale[0] *= -1.f;
	this->UpdateUnitVectors();
}

void ott::Sprite::FlipVertical() {
	this->scale[1] *= -1.f;
	this->UpdateUnitVectors();
}

void ott::Sprite::Draw() {
	if (shader != nullptr) {
		shader->EnableShader(this);
		shader->SetMatrix2("mRotation", rotation);
		shader->SetFloat("fScaleX", scale[0]);
		shader->SetFloat("fScaleY", scale[1]);
		//shader->SetVector2("vUnitX", uX);
		//shader->SetVector2("vUnitY", uY);
		shader->SetVector2("vCenter", center);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)rawOffsets[0]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const void*)rawOffsets[1]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		shader->DisableShader(this);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, nContext);
		glDisable(GL_TEXTURE_2D);
	}
}

void ott::Sprite::UpdateUnitVectors() {

	uX = rotation.Transform(Vector2(scale[0], 0));
	uY = rotation.Transform(Vector2(0, scale[1]));
}

void ott::Sprite::SetupGeometry(bool reverse_winding) {

	std::vector<std::vector<float> > rawData;
	if (reverse_winding == false) {
		rawData.push_back({ 1,  1, 0,
						    1, -1, 0,
                           -1, -1, 0,
                           -1,  1, 0}); // (x,y,z)
		rawData.push_back({1, 0,
                           1, 1,
						   0, 1,
						   0, 0}); // (u,v)
	}
	else {
		rawData.push_back({-1,  1, 0,
                           -1, -1, 0,
                            1, -1, 0,
							1,  1, 0}); // (x,y,z)
		rawData.push_back({0, 0,
						   0, 1,
						   1, 1,
						   1, 0}); // (u,v)
	}
	this->vertexVBO = ott::GenerateVertexBufferObject(rawData, rawOffsets);
}

