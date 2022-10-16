#ifndef GRAPHICS_3D_OTT_SHADER_3D_HPP
#define GRAPHICS_3D_OTT_SHADER_3D_HPP

#include <graphics/core/OTTShader.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ott {

namespace shaders {

extern const std::vector<std::string> vertexSimple;
extern const std::vector<std::string> fragmentSimple;

extern const std::vector<std::string> vertexDefault;
extern const std::vector<std::string> fragmentDefault;

extern const std::vector<std::string> vertexColor;
extern const std::vector<std::string> fragmentColor;

extern const std::vector<std::string> vertexNormal;
extern const std::vector<std::string> fragmentNormal;

extern const std::vector<std::string> vertexZDepth;
extern const std::vector<std::string> fragmentZDepth;

extern const std::vector<std::string> vertexTexture;
extern const std::vector<std::string> fragmentTexture;

class DefaultShader : public Shader {
public:
	DefaultShader() :
		Shader(),
		good(false),
		type(ShaderType::None),
		name("none")
	{
	}

	DefaultShader(const ShaderType& stype_);

	bool Good() const { 
		return good; 
	}

	ShaderType Type() const { 
		return type; 
	}

private:
	bool good;

	ShaderType type;

	std::string name;

	bool Generate(const std::vector<std::string>& vert, const std::vector<std::string>& frag);
};

class ShaderList {
public:
	typedef std::map<ShaderType, std::unique_ptr<DefaultShader> > ShaderMap;

	typedef std::map<ShaderType, std::unique_ptr<DefaultShader> >::iterator ShaderMapIterator;

	/** Default constructor (empty list)
	  */
	ShaderList() :
		shaders()
	{
	}

	/** Get a pointer to a shader in the list of defined shaders
	  * If the shader does not exist, attempt to create it
	  */
	Shader* Get(const ShaderType& type) {
		ShaderMapIterator iter = shaders.find(type);
		if (iter != shaders.end()) {
			return (iter->second.get());
		}
		DefaultShader* shdr = new DefaultShader(type);
		shaders[type] = std::unique_ptr<DefaultShader>(shdr);
		return shdr;
	}

private:
	ShaderMap shaders;
};

} /* namespace shaders */
} /* namespace ott */

#endif // #ifndef GRAPHICS_3D_OTT_SHADER_3D_HPP
