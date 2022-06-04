#ifndef GRAPHICS_OTT_TEXTURE_MANAGER_HPP
#define GRAPHICS_OTT_TEXTURE_MANAGER_HPP

#include <vector>
#include <memory>
#include <string>

namespace ott {

class Object;
class Texture;

class TextureManager {
public:
	/** Copy constructor
	  */
	TextureManager(const TextureManager&) = delete;

	/** Destructor
	  */
	~TextureManager();

	/** Assignment operator
	  */
	TextureManager& operator = (const TextureManager&) = delete;

	/** Get reference to singleton
	  */
	static TextureManager& Instance() {
		static TextureManager instance;
		return instance;
	}

	Texture* Last() {
		if (textures.empty())
			return 0x0;
		return textures.back().get();
	}

	/** Get the number of loaded textures
	  */
	size_t NumTextures() const {
		return textures.size();
	}

	/** Load a new texture and add it to the list of textures
	  */
	bool Load(const std::string& fname, const std::string& name = "");

	/** Add a texture to the list, TextureManager will take ownership of the object
	  */
	bool Add(Texture* tex);

	/** Set the texture of an object by name
	  */
	bool SetTexture(Object* obj, const std::string& name);

	/** Set the texture of an object by OpenGL texture context ID
	  */
	bool SetTexture(Object* obj, const uint32_t& id);

	/** Set the texture of an object to the most recently loaded texture
	  */
	bool SetTexture(Object* obj);

	/** Find texture by name string
	  */
	Texture* FindByName(const std::string& name);

	/** Find texture by OpenGL texture context ID
	  */
	Texture* FindByID(const uint32_t& id);

private:
	std::vector<std::unique_ptr<Texture> > textures; ///< Vector of pointers to all currently loaded textures

	/** Default constructor
	  */
	TextureManager() :
		textures()
	{
	}
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_TEXTURE_MANAGER_HPP
