#ifndef OTT_SHADER_3D_HPP
#define OTT_SHADER_3D_HPP

#include <string>

#include "OTTShader.hpp"

class OTTShader3D : public OTTShader {
public:
	OTTShader3D() :
		OTTShader(),
		enableFunc(&defaultShaderEnable),
		disableFunc(&defaultShaderDisable)
	{
	}

	OTTShader3D(const std::string& vert, const std::string& frag) :
		OTTShader(),
		enableFunc(&defaultShaderEnable),
		disableFunc(&defaultShaderDisable)
	{
		generate(vert, frag);
	}

	void setShaderEnableFunction(shaderStateFunction func) {
		enableFunc = func;
	}

	void setShaderDisableFunction(shaderStateFunction func) {
		disableFunc = func;
	}

	void enableObjectShader(const object* obj) const;

	void disableObjectShader(const object* obj) const;

	static void defaultShaderEnable(OTTShader*, const object*);

	static void defaultShaderDisable(OTTShader*, const object*);

	static void bindObjectTexture(OTTShader*, const object* obj);

	static void unbindObjectTexture(OTTShader*, const object*);

protected:
	shaderStateFunction enableFunc;

	shaderStateFunction disableFunc;

	virtual void onUserShaderEnable(const object* obj) const {
	}

	virtual void onUserShaderDisable(const object* obj) const {
	}
};

namespace OTTDefaultShaders {
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

	class DefaultShader : public OTTShader3D {
	public:
		DefaultShader() :
			OTTShader3D(),
			good(false),
			type(ShaderType::NONE),
			name("none")
		{
		}

		DefaultShader(const ShaderType& stype_);

		bool isGood() const { return good; }

		ShaderType getType() const { return type; }

	private:
		bool good;

		ShaderType type;

		std::string name;

		bool generate(const std::vector<std::string>& vert, const std::vector<std::string>& frag);
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
		OTTShader3D* get(const ShaderType& type) {
			ShaderMapIterator iter = shaders.find(type);
			if (iter != shaders.end())
				return (iter->second.get());
			DefaultShader* shdr = new DefaultShader(type);
			shaders[type] = std::unique_ptr<DefaultShader>(shdr);
			return shdr;
		}

	private:
		ShaderMap shaders;
	};
}

#endif // ifndef OTT_SHADER_3D_HPP