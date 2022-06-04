#ifndef GRAPHICS_OTT_SHADER_HPP
#define GRAPHICS_OTT_SHADER_HPP

#include <cstdint>
#include <string>

namespace ott {

class Vector2;
class Vector3;
class Vector4;
class Matrix2;
class Matrix3;
class Matrix4;

class Object;
class Shader;

typedef void (*ShaderStateFunction_t)(Shader*, const Object*);

/// <summary>
/// Default shader types
/// </summary>
enum class ShaderType {
	None,    ///< No shader
	Simple,  ///< Simplest OpenGL shader
	Default, ///< Simple vertex and fragment shader. Fragment color taken from gl_Color
	Color,   ///< Fragment color interpolated from vertex colors
	Normal,  ///< Shader showing differences in polygon normals
	ZDepth,  ///< Shader showing differences in pixel Z-depth computed by OpenGL
	Texture  ///< Use Object texture
};

class Shader {
public:
	Shader() :
		bHidden(false),
		nVertShader(0),
		nGeomShader(0),
		nFragShader(0),
		nProgram(0),
		shaderData(0x0)
	{
	}

	Shader(const std::string& vert, const std::string& frag) :
		bHidden(false),
		nVertShader(0),
		nGeomShader(0),
		nFragShader(0),
		nProgram(0),
		shaderData(0x0)
	{
		this->Generate(vert, frag);
	}

	~Shader();

	bool IsHidden() const {
		return bHidden;
	}

	void Hide() {
		bHidden = true;
	}

	void Show() {
		bHidden = false;
	}

	void EnableShader() const;

	void DisableShader() const;

	uint32_t Program() const {
		return nProgram;
	}

	void* ShaderDataPointer() {
		return shaderData;
	}

	/** Generate a shader program containing a vertex and fragment shader
	  * @return True if the shader program is compiled successfully
	  */
	bool Generate(const std::string& vert, const std::string& frag);

	/** Generate a shader program containing a vertex, geometry, and fragment shader
	  * @return True if the shader program is compiled successfully
	  */
	bool Generate(const std::string& vert, const std::string& geom, const std::string& frag);

	void SetShaderDataPointer(void* ptr) {
		shaderData = ptr;
	}

	void SetBool(const std::string& name, const bool& value) const;

	void SetInt(const std::string& name, const int32_t& value) const;

	void SetFloat(const std::string& name, const float& value) const;

	void SetVector2(const std::string& name, const Vector2& value) const;

	void SetVector2(const std::string& name, const Vector2* value) const;

	void SetVector2(const std::string& name, const float& x, const float& y) const;

	void SetVector3(const std::string& name, const Vector3& value) const;

	void SetVector3(const std::string& name, const Vector3* value) const;

	void SetVector3(const std::string& name, const float& x, const float& y, const float& z) const;

	void SetVector4(const std::string& name, const Vector4& value) const;

	void SetVector4(const std::string& name, const Vector4* value) const;

	void SetVector4(const std::string& name, const float& x, const float& y, const float& z, const float& w) const;

	void SetMatrix2(const std::string& name, const Matrix2& value) const;

	void SetMatrix2(const std::string& name, const Matrix2* mat) const;

	void SetMatrix2(const std::string& name, const float* mat) const;

	void SetMatrix3(const std::string& name, const Matrix3& value) const;

	void SetMatrix3(const std::string& name, const Matrix3* mat) const;

	void SetMatrix3(const std::string& name, const float* mat) const;

	void SetMatrix4(const std::string& name, const Matrix4& value) const;

	void SetMatrix4(const std::string& name, const Matrix4* mat) const;

	void SetMatrix4(const std::string& name, const float* mat) const;

	int32_t GetUniformLocation(const std::string& name) const;

protected:
	bool bHidden; ///< Set if any Object with this shader should be hidden

	uint32_t nVertShader; ///< OpenGL vertex shader context

	uint32_t nGeomShader; ///< OpenGL geometry shader context

	uint32_t nFragShader; ///< OpenGL fragment shader context

	uint32_t nProgram; ///< OpenGL program ID

	void* shaderData;

	bool ReadShader(const std::string& fname, std::string& retval);

	bool CompileShader(const uint32_t& nShader, const std::string& sBody);

	/** Read and compile input vertex shader body
	  * @return True if shader is compiled successfully
	  */
	bool BuildVertexShader(const std::string& fname);

	/** Read and compile input geometry shader body
	  * @return True if shader is compiled successfully
	  */
	bool BuildGeometryShader(const std::string& fname);

	/** Read and compile input fragment shader body
	  * @return True if shader is compiled successfully
	  */
	bool BuildFragmentShader(const std::string& fname);

	bool GenerateProgram();
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_OTT_SHADER_HPP
