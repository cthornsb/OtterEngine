#include "OTTTextureManager.hpp"
#include "OTTTexture.hpp"

#include <graphics/geometry/OTTObject.hpp>

ott::TextureManager::~TextureManager() {

}

bool ott::TextureManager::Load(const std::string& fname, const std::string& name/*=""*/) {
	textures.push_back(std::unique_ptr<Texture>(new Texture(fname, name)));
	Texture* tex = textures.back().get();
	if (tex->Good() == false) { // Failed to load texture, pop it off the back of the vector
		textures.pop_back();
		return false;
	}
	tex->GetTexture();
	return true;
}

bool ott::TextureManager::Add(Texture* tex) {
	if (!tex || tex->Good() == false) {
		return false;
	}
	tex->GetTexture();
	textures.push_back(std::unique_ptr<Texture>(tex));
	return true;
}

bool ott::TextureManager::SetTexture(Object* obj, const std::string& name) {
	Texture* tex = this->FindByName(name);
	if (!tex)
		return false;
	obj->SetTexture(tex);
	return true;
}

bool ott::TextureManager::SetTexture(Object* obj, const uint32_t& id) {
	Texture* tex = this->FindByID(id);
	if (!tex)
		return false;
	obj->SetTexture(tex);
	return true;
}

bool ott::TextureManager::SetTexture(Object* obj) {
	if (textures.empty())
		return false;
	obj->SetTexture(textures.back().get());
	return true;
}

ott::Texture* ott::TextureManager::FindByName(const std::string& name) {
	if (textures.empty())
		return 0x0;
	for (auto tex = textures.cbegin(); tex != textures.cend(); tex++) {
		if (tex->get()->Name() == name)
			return tex->get();
	}
	return 0x0;
}

ott::Texture* ott::TextureManager::FindByID(const uint32_t& id) {
	if (textures.empty())
		return 0x0;
	for (auto tex = textures.cbegin(); tex != textures.cend(); tex++) {
		if (tex->get()->Context() == id)
			return tex->get();
	}
	return 0x0;
}
