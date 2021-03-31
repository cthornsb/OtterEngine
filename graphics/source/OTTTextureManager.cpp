#include "OTTTextureManager.hpp"
#include "OTTTexture.hpp"

#include "object.hpp"

OTTTextureManager::~OTTTextureManager() {

}

bool OTTTextureManager::loadTexture(const std::string& fname, const std::string& name/*=""*/) {
	textures.push_back(std::unique_ptr<OTTTexture>(new OTTTexture(fname, name)));
	OTTTexture* tex = textures.back().get();
	if (!tex->isGood()) { // Failed to load texture, pop it off the back of the vector
		textures.pop_back();
		return false;
	}
	tex->getTexture();
	return true;
}

bool OTTTextureManager::addTexture(OTTTexture* tex) {
	if (!tex || !tex->isGood())
		return false;
	tex->getTexture();
	textures.push_back(std::unique_ptr<OTTTexture>(tex));
	return true;
}

bool OTTTextureManager::setTexture(object* obj, const std::string& name) {
	OTTTexture* tex = findByName(name);
	if (!tex)
		return false;
	obj->setTexture(tex);
	return true;
}

bool OTTTextureManager::setTexture(object* obj, const unsigned int& id) {
	OTTTexture* tex = findByID(id);
	if (!tex)
		return false;
	obj->setTexture(tex);
	return true;
}

bool OTTTextureManager::setTexture(object* obj) {
	if (textures.empty())
		return false;
	obj->setTexture(textures.back().get());
	return true;
}

OTTTexture* OTTTextureManager::findByName(const std::string& name) {
	if (textures.empty())
		return 0x0;
	for (auto tex = textures.cbegin(); tex != textures.cend(); tex++) {
		if (tex->get()->getName() == name)
			return tex->get();
	}
	return 0x0;
}

OTTTexture* OTTTextureManager::findByID(const unsigned int& id) {
	if (textures.empty())
		return 0x0;
	for (auto tex = textures.cbegin(); tex != textures.cend(); tex++) {
		if (tex->get()->getContext() == id)
			return tex->get();
	}
	return 0x0;
}
