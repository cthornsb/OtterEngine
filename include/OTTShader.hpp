#ifndef SHADER_HPP
#define SHADER_HPP

#include <memory> // unique_ptr
#include <string>
#include <map>

#include "Vector.hpp"
#include "Matrix.hpp"

class object;

class OTTShader;

typedef void (*shaderStateFunction)(OTTShader*, const object*);

/// <summary>
/// Default shader types
/// </summary>
enum class ShaderType {
	NONE,    ///< No shader
	SIMPLE,  ///< Simplest OpenGL shader
	DEFAULT, ///< Simple vertex and fragment shader. Object color taken from gl_Color
	COLOR,   ///< Fragment color interpolated from vertex colors
	NORMAL,  ///< Shader showing differences in polygon normals
	ZDEPTH,  ///< Shader showing differences in pixel Z-depth computed by OpenGL
	TEXTURE  ///< Use object texture
};

class OTTShader {
public:
	OTTShader() :
		bHidden(false),
		nVertShader(0),
		nGeomShader(0),
		nFragShader(0),
		nProgram(0),
		enableFunc(&defaultShaderEnable),
		disableFunc(&defaultShaderDisable),
		shaderData(0x0)
	{ 
	}

	OTTShader(const std::string& vert, const std::string& frag) :
		bHidden(false),
		nVertShader(0),
		nGeomShader(0),
		nFragShader(0),
		nProgram(0),
		enableFunc(&defaultShaderEnable),
		disableFunc(&defaultShaderDisable),
		shaderData(0x0)
	{
		generate(vert, frag);
	}

	~OTTShader();

	bool isHidden() const {
		return bHidden;
	}

	void hide() {
		bHidden = true;
	}

	void show() {
		bHidden = false;
	}

	void enableShader() const;

	void enableObjectShader(const object* obj) const;

	void disableShader() const;

	void disableObjectShader(const object* obj) const;

	unsigned int getProgram() const { 
		return nProgram; 
	}

	void* getShaderDataPointer() {
		return shaderData;
	}

	/** Generate a shader program containing a vertex and fragment shader
	  * @return True if the shader program is compiled successfully
	  */
	bool generate(const std::string& vert, const std::string& frag);

	/** Generate a shader program containing a vertex, geometry, and fragment shader
	  * @return True if the shader program is compiled successfully
	  */
	bool generate(const std::string& vert, const std::string& geom, const std::string& frag);

	void setShaderEnableFunction(shaderStateFunction func) {
		enableFunc = func;
	}

	void setShaderDisableFunction(shaderStateFunction func) {
		disableFunc = func;
	}

	void setShaderDataPointer(void* ptr) {
		shaderData = ptr;
	}

	void setBool(const std::string& name, const bool& value) const;

	void setInt(const std::string& name, const int& value) const;

	void setFloat(const std::string& name, const float& value) const;

	void setVector2(const std::string& name, const Vector2& value) const;

	void setVector2(const std::string& name, const Vector2* value) const;

	void setVector2(const std::string& name, const float& x, const float& y) const;

	void setVector3(const std::string& name, const Vector3& value) const;

	void setVector3(const std::string& name, const Vector3* value) const;

	void setVector3(const std::string& name, const float& x, const float& y, const float& z) const;

	void setVector4(const std::string& name, const Vector4& value) const;

	void setVector4(const std::string& name, const Vector4* value) const;

	void setVector4(const std::string& name, const float& x, const float& y, const float& z, const float& w) const;

	void setMatrix2(const std::string& name, const Matrix2& value) const;

	void setMatrix2(const std::string& name, const Matrix2* mat) const;

	void setMatrix2(const std::string& name, const float* mat) const;

	void setMatrix3(const std::string& name, const Matrix3& value) const;

	void setMatrix3(const std::string& name, const Matrix3* mat) const;

	void setMatrix3(const std::string& name, const float* mat) const;

	void setMatrix4(const std::string& name, const Matrix4& value) const;

	void setMatrix4(const std::string& name, const Matrix4* mat) const;

	void setMatrix4(const std::string& name, const float* mat) const;

	int getUniformLocation(const std::string& name) const ;

	static void defaultShaderEnable(OTTShader*, const object*);

	static void defaultShaderDisable(OTTShader*, const object*);

	static void bindObjectTexture(OTTShader*, const object* obj);

	static void unbindObjectTexture(OTTShader*, const object*);

protected:
	bool bHidden; ///< Set if any object with this shader should be hidden

	unsigned int nVertShader; ///< OpenGL vertex shader context

	unsigned int nGeomShader; ///< OpenGL geometry shader context

	unsigned int nFragShader; ///< OpenGL fragment shader context

	unsigned int nProgram; ///< OpenGL program ID

	shaderStateFunction enableFunc;

	shaderStateFunction disableFunc;

	void* shaderData;

	bool readShader(const std::string& fname, std::string& retval);

	bool compileShader(const unsigned int& nShader, const std::string& sBody);

	/** Read and compile input vertex shader body
	  * @return True if shader is compiled successfully
	  */
	bool buildVertexShader(const std::string& fname);

	/** Read and compile input geometry shader body
	  * @return True if shader is compiled successfully
	  */
	bool buildGeometryShader(const std::string& fname);

	/** Read and compile input fragment shader body
	  * @return True if shader is compiled successfully
	  */
	bool buildFragmentShader(const std::string& fname);

	bool generateProgram();

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

	class DefaultShader : public OTTShader {
	public:
		DefaultShader() :
			OTTShader(),
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
