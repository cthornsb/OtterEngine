#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>

#include "Vector.hpp"
#include "Matrix.hpp"

/// <summary>
/// Default shader types
/// </summary>
enum class ShaderType {
	NONE,    ///< No shader
	DEFAULT, ///< Simple vertex and fragment shader. Objects appear solid white
	MVP,     ///< Model, view, and projection matrix transformation shader
	NORMAL,  ///< Shader showing differences in polygon normals
	ZDEPTH   ///< Shader showing differences in pixel Z-depth computed by OpenGL
};

class Shader {
public:
	Shader() :
		nVertShader(0),
		nFragShader(0),
		nProgram(0)
	{ 
	}

	Shader(const std::string& vert, const std::string& frag) :
		nVertShader(0),
		nFragShader(0),
		nProgram(0)
	{
		generate(vert, frag);
	}

	~Shader();

	void useShader();

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
};

namespace DefaultShaders {
	extern const size_t vertexDefaultLength;
	extern const size_t fragmentDefaultLength;

	extern const std::string vertexDefault[];
	extern const std::string fragmentDefault[];

	extern const size_t vertexMVPLength;
	extern const size_t fragmentMVPLength;

	extern const std::string vertexMVP[];
	extern const std::string fragmentMVP[];

	extern const size_t vertexNormalLength;
	extern const size_t fragmentNormalLength;

	extern const std::string vertexNormal[];
	extern const std::string fragmentNormal[];

	extern const size_t vertexZDepthLength;
	extern const size_t fragmentZDepthLength;

	extern const std::string vertexZDepth[];
	extern const std::string fragmentZDepth[];

	class DefaultShader : public Shader {
	public:
		DefaultShader() :
			Shader(),
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

		bool generate(const std::string* vert, const size_t& nLinesVert, const std::string* frag, const size_t& nLinesFrag);
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
		Shader* get(const ShaderType& type) {
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
};

#endif