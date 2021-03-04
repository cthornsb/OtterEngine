#ifndef SHADER_HPP
#define SHADER_HPP

#include <memory> // unique_ptr
#include <string>
#include <map>

#include "Vector.hpp"
#include "Matrix.hpp"

class object;

/// <summary>
/// Default shader types
/// </summary>
enum class ShaderType {
	NONE,    ///< No shader
	DEFAULT, ///< Simple vertex and fragment shader. Object color taken from gl_Color
	COLOR,   ///< Fragment color interpolated from vertex colors
	NORMAL,  ///< Shader showing differences in polygon normals
	ZDEPTH,  ///< Shader showing differences in pixel Z-depth computed by OpenGL
	TEXTURE  ///< Use object texture
};

class OTTShader {
public:
	OTTShader() :
		nVertShader(0),
		nFragShader(0),
		nProgram(0)
	{ 
	}

	OTTShader(const std::string& vert, const std::string& frag) :
		nVertShader(0),
		nFragShader(0),
		nProgram(0)
	{
		generate(vert, frag);
	}

	~OTTShader();

	void enableShader() const;

	void enableObjectShader(const object* obj) const;

	void disableShader() const;

	void disableObjectShader(const object* obj) const;

	unsigned int getProgram() const { return nProgram; }

	bool generate(const std::string& vert, const std::string& frag);

	void setBool(const std::string& name, const bool& value) const;

	void setInt(const std::string& name, const int& value) const;

	void setFloat(const std::string& name, const float& value) const;

	void setVector2(const std::string& name, const Vector2* value) const;

	void setVector2(const std::string& name, const float& x, const float& y) const;

	void setVector3(const std::string& name, const Vector3* value) const;

	void setVector3(const std::string& name, const float& x, const float& y, const float& z) const;

	void setVector4(const std::string& name, const Vector4* value) const;

	void setVector4(const std::string& name, const float& x, const float& y, const float& z, const float& w);

	void setMatrix2(const std::string& name, const Matrix2* mat) const;

	void setMatrix2(const std::string& name, const float* mat) const;

	void setMatrix3(const std::string& name, const Matrix3* mat) const;

	void setMatrix3(const std::string& name, const float* mat) const;

	void setMatrix4(const std::string& name, const Matrix4* mat) const;

	void setMatrix4(const std::string& name, const float* mat) const;

protected:
	unsigned int nVertShader; ///< OpenGl shader context ID
	unsigned int nFragShader; ///< OpenGl shader context ID

	unsigned int nProgram; ///< OpenGl program ID

	bool readShader(const std::string& fname, std::string& retval);

	bool compileShader(const unsigned int& nShader, const std::string& sBody);

	bool generateProgram();

	virtual void onShaderEnable(const object*) const { }

	virtual void onShaderDisable(const object*) const { }
};

namespace OTTDefaultShaders {
	typedef void (*shaderStateFunction)(const object*);

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

	void defaultShaderEnable(const object*);

	void defaultShaderDisable(const object*);

	void bindObjectTexture(const object* obj);

	void unbindObjectTexture(const object*);

	class DefaultShader : public OTTShader {
	public:
		DefaultShader() :
			OTTShader(),
			good(false),
			type(ShaderType::NONE),
			name("none"),
			enableFunc(&defaultShaderEnable),
			disableFunc(&defaultShaderDisable)
		{
		}

		DefaultShader(const ShaderType& stype_);

		bool isGood() const { return good; }

		ShaderType getType() const { return type; }

	private:
		bool good;

		ShaderType type;

		std::string name;

		shaderStateFunction enableFunc;

		shaderStateFunction disableFunc;

		bool generate(const std::vector<std::string>& vert, const std::vector<std::string>& frag);

		virtual void onShaderEnable(const object* obj) const { 
			(*enableFunc)(obj);
		}

		virtual void onShaderDisable(const object* obj) const {
			(*disableFunc)(obj);
		}
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
		OTTShader* get(const ShaderType& type) {
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

#endif
