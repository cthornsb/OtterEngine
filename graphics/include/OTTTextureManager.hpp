#ifndef OTT_TEXTURE_CONTAINER_HPP
#define OTT_TEXTURE_CONTAINER_HPP

#include <vector>
#include <memory>
#include <string>

class OTTTexture;

class object;

class OTTTextureManager {
public:
	/** Copy constructor
	  */
	OTTTextureManager(const OTTTextureManager&) = delete;

	/** Destructor
	  */
	~OTTTextureManager();

	/** Assignment operator
	  */
	OTTTextureManager& operator = (const OTTTextureManager&) = delete;

	/** Get reference to singleton
	  */
	static OTTTextureManager& getInstance() {
		static OTTTextureManager instance;
		return instance;
	}

	OTTTexture* getLast() {
		if (textures.empty())
			return 0x0;
		return textures.back().get();
	}

	/** Get the number of loaded textures
	  */
	size_t getNumTextures() const {
		return textures.size();
	}

	/** Load a new texture and add it to the list of textures
	  */
	bool loadTexture(const std::string& fname, const std::string& name = "");

	/** Add a texture to the list, OTTTextureManager will take ownership of the object
	  */
	bool addTexture(OTTTexture* tex);

	/** Set the texture of an object by name
	  */
	bool setTexture(object* obj, const std::string& name);

	/** Set the texture of an object by OpenGL texture context ID
	  */
	bool setTexture(object* obj, const unsigned int& id);

	/** Set the texture of an object to the most recently loaded texture
	  */
	bool setTexture(object* obj);

	/** Find texture by name string
	  */
	OTTTexture* findByName(const std::string& name);

	/** Find texture by OpenGL texture context ID
	  */
	OTTTexture* findByID(const unsigned int& id);

private:
	std::vector<std::unique_ptr<OTTTexture> > textures; ///< Vector of pointers to all currently loaded textures

	/** Default constructor
	  */
	OTTTextureManager() :
		textures()
	{
	}
};

#endif // ifndef OTT_TEXTURE_CONTAINER_HPP