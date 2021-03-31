#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>

#include "OTTShader.hpp"
#include "OTTTexture.hpp"
#include "object.hpp"

OTTShader::~OTTShader() {
	glDeleteShader(nVertShader);
	glDeleteShader(nFragShader);
	glDeleteProgram(nProgram);
}

void OTTShader::enableShader() const {
	glUseProgram(nProgram);
}

void OTTShader::enableObjectShader(const object* obj) const {
	onShaderEnable(obj);
}

void OTTShader::disableShader() const {
	glUseProgram(0);
}

void OTTShader::disableObjectShader(const object* obj) const {
	onShaderDisable(obj);
}

bool OTTShader::generate(const std::string& vert, const std::string& frag) {
	std::string sBody;

	// Create the vertex shader
	if (!readShader(vert, sBody))
		return false;
	nVertShader = glCreateShader(GL_VERTEX_SHADER);
	if (!compileShader(nVertShader, sBody))
		return false;

	// Create the fragment shader
	if (!readShader(frag, sBody))
		return false;
	nFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!compileShader(nFragShader, sBody))
		return false;

	// Generate the OpenGL shader program
	return generateProgram();
}

bool OTTShader::generateProgram() {
	// Create the shader program
	int retval;
	nProgram = glCreateProgram();

	// Bind default attribtue locations (equivalent to "layout (location = N)")
	glBindAttribLocation(nProgram, 0, "vPosition");
	glBindAttribLocation(nProgram, 1, "vNormal");
	glBindAttribLocation(nProgram, 2, "vColor");
	glBindAttribLocation(nProgram, 3, "vTexture");

	// Link shader program
	glAttachShader(nProgram, nVertShader);
	glAttachShader(nProgram, nFragShader);
	glLinkProgram(nProgram);
	glGetProgramiv(nProgram, GL_LINK_STATUS, &retval);
	if (retval != GL_TRUE) {
		std::cout << " [Shader] Error! Failed to create shader program" << std::endl;
		char log[512];
		glGetProgramInfoLog(nProgram, 512, 0x0, log);
		std::cout << " [Shader]  output log:" << std::endl; // Print compiler output log
		std::cout << log << std::endl;
		return false;
	}
	std::cout << " [Shader] debug: nVertShader=" << nVertShader << std::endl;
	std::cout << " [Shader] debug: nFragShader=" << nFragShader << std::endl;
	std::cout << " [Shader] debug: nProgram=" << nProgram << std::endl;

	return true;
}

void OTTShader::setBool(const std::string& name, const bool& value) const {
	glUniform1i(glGetUniformLocation(nProgram, name.c_str()), (int)value);
}

void OTTShader::setInt(const std::string& name, const int& value) const {
	glUniform1i(glGetUniformLocation(nProgram, name.c_str()), value);
}

void OTTShader::setFloat(const std::string& name, const float& value) const {
	glUniform1f(glGetUniformLocation(nProgram, name.c_str()), value);
}

void OTTShader::setVector2(const std::string& name, const Vector2* vec) const {
	glUniform2fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void OTTShader::setVector2(const std::string& name, const float& x, const float& y) const {
	glUniform2f(glGetUniformLocation(nProgram, name.c_str()), x, y);
}

void OTTShader::setVector3(const std::string& name, const Vector3* vec) const {
	glUniform3fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void OTTShader::setVector3(const std::string& name, const float& x, const float& y, const float& z) const {
	glUniform3f(glGetUniformLocation(nProgram, name.c_str()), x, y, z);
}

void OTTShader::setVector4(const std::string& name, const Vector4* vec) const {
	glUniform4fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void OTTShader::setVector4(const std::string& name, const float& x, const float& y, const float& z, const float& w) {
	glUniform4f(glGetUniformLocation(nProgram, name.c_str()), x, y, z, w);
}

void OTTShader::setMatrix2(const std::string& name, const Matrix2* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void OTTShader::setMatrix2(const std::string& name, const float* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void OTTShader::setMatrix3(const std::string& name, const Matrix3* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void OTTShader::setMatrix3(const std::string& name, const float* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void OTTShader::setMatrix4(const std::string& name, const Matrix4* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void OTTShader::setMatrix4(const std::string& name, const float* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

bool OTTShader::readShader(const std::string& fname, std::string& retval) {
	retval = "";
	std::string line;
	std::ifstream input(fname.c_str());
	if (!input.good())
		return false;
	while (true) {
		std::getline(input, line);
		retval += line + "\n";
		if (input.eof())
			break;
	}
	input.close();
	return true;
}

bool OTTShader::compileShader(const unsigned int& nShader, const std::string& sBody) {
	int retval;
	const char* cptr = sBody.data();
	int nLength = (int)sBody.length();
	glShaderSource(nShader, 1, &cptr, &nLength);
	glCompileShader(nShader);
	glGetShaderiv(nShader, GL_COMPILE_STATUS, &retval);
	if (retval != GL_TRUE) {
		std::cout << " [Shader] Error! Failed to compile shader" << std::endl;
		char log[512];
		glGetShaderInfoLog(nVertShader, 512, 0x0, log);
		std::cout << " [debug]  output log:" << std::endl; // Print compiler output log
		std::cout << log << std::endl;
		return false;
	}
	return true;
}

OTTDefaultShaders::DefaultShader::DefaultShader(const ShaderType& stype_) :
	OTTShader(),
	good(false),
	type(stype_),
	name("none"),
	enableFunc(&defaultShaderEnable),
	disableFunc(&defaultShaderDisable)
{
	switch (stype_) {
	case ShaderType::NONE:
		break;
	case ShaderType::SIMPLE:
		name = "simple";
		good = generate(vertexSimple, fragmentSimple);
		break;
	case ShaderType::DEFAULT:
		name = "default";
		good = generate(vertexDefault, fragmentDefault);
		break;
	case ShaderType::COLOR:
		name = "color";
		good = generate(vertexColor, fragmentColor);
		break;
	case ShaderType::NORMAL:
		name = "normal";
		good = generate(vertexNormal, fragmentNormal);
		break;
	case ShaderType::ZDEPTH:
		name = "zdepth";
		good = generate(vertexZDepth, fragmentZDepth);
		break;
	case ShaderType::TEXTURE:
		name = "texture";
		good = generate(vertexTexture, fragmentTexture);
		enableFunc = &bindObjectTexture;
		disableFunc = &unbindObjectTexture;
		break;
	default:
		break;
	}
}

bool OTTDefaultShaders::DefaultShader::generate(const std::vector<std::string>& vert, const std::vector<std::string>& frag) {
	// Create the vertex shader
	std::string vertBody;
	for (auto line = vert.cbegin(); line != vert.cend(); line++)
		vertBody += *line;
	nVertShader = glCreateShader(GL_VERTEX_SHADER);
	if (!compileShader(nVertShader, vertBody)) {
		std::cout << " [DefaultShaders] Error! Failed to generate vertex shader for default shader name=" << name << std::endl;
		return false;
	}

	// Create the fragment shader
	std::string fragBody;
	for (auto line = frag.cbegin(); line != frag.cend(); line++)
		fragBody += *line;
	nFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!compileShader(nFragShader, fragBody)) {
		std::cout << " [DefaultShaders] Error! Failed to generate fragment shader for default shader name=" << name << std::endl;
		return false;
	}

	// Generate the OpenGL shader program
	return generateProgram();
}

const std::vector<std::string> OTTDefaultShaders::vertexSimple = {
	"void main() {\n",
	"	gl_Position = ftransform();\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::fragmentSimple = {
	"void main() {\n",
	"	gl_FragColor = vec4(1,1,1,1);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::vertexDefault = {
	"in vec3 vPosition;\n",
	"uniform mat4 MVP;\n",
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	vertexColor = gl_Color;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::fragmentDefault = {
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	gl_FragColor = vec4(vertexColor, 1.f);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::vertexColor = {
	"in vec3 vPosition;\n",
	"in vec3 vColor;\n",
	"uniform mat4 MVP;\n",
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	vertexColor = vColor;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::fragmentColor = {
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	gl_FragColor = vec4(vertexColor, 1.f);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::vertexNormal = {
	"in vec3 vPosition;\n",
	"in vec3 vNormal;\n",
	"uniform mat4 MVP;\n",
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	vertexColor = vNormal;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::fragmentNormal = {
	"varying vec3 vertexColor;\n",
	"void main() {\n",
	"	gl_FragColor = vec4(vertexColor, 1.0);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::vertexZDepth = {
	"in vec3 vPosition;\n",
	"uniform mat4 MVP;\n",
	"void main() {\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::fragmentZDepth = {
	"float near = 0.1f;\n",
	"float far = 10.0f;\n",
	"float LinearizeDepth(float depth) {\n",
	"	float z = 2.0 * depth - 1.0;\n",
	"	return (2.0 * near * far / (far + near - z * (far - near)));\n",
	"}\n",
	"void main() {\n",
	"	float depth = LinearizeDepth(gl_FragCoord.z) / (far-near);\n",
	"	gl_FragColor = vec4(vec3(1.f-depth), 1.0);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::vertexTexture = {
	"in vec3 vPosition;\n",
	"in vec2 vTexture;\n",
	"varying vec2 uvCoords;",
	"uniform mat4 MVP;\n",
	"void main() {\n",
	"	uvCoords = vTexture;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> OTTDefaultShaders::fragmentTexture = {
	"varying vec2 uvCoords;\n",
	"uniform sampler2D sampler;\n",
	"void main() {\n",
	"	vec4 sampleColor = texture(sampler, uvCoords);\n",
	"	if (sampleColor.a == 0.f)\n",
	"		discard;\n",
	"	gl_FragColor = sampleColor;\n",
	"}\n"
};

void OTTDefaultShaders::defaultShaderEnable(const object*) {
}

void OTTDefaultShaders::defaultShaderDisable(const object*) {
}

void OTTDefaultShaders::bindObjectTexture(const object* obj) {
	// Bind object texture (if available)
	if (obj->getTexture())
		glBindTexture(GL_TEXTURE_2D, obj->getTexture()->getContext());
}

void OTTDefaultShaders::unbindObjectTexture(const object*) {
	// Unbind OpenGL texture
	glBindTexture(GL_TEXTURE_2D, 0);
}
