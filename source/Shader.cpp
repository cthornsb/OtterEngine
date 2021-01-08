#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>

#include "Shader.hpp"

Shader::~Shader() {
	glDeleteShader(nVertShader);
	glDeleteShader(nFragShader);
	glDeleteProgram(nProgram);
}

void Shader::useShader() {
	glUseProgram(nProgram);
}

bool Shader::generate(const std::string& vert, const std::string& frag) {
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

bool Shader::generateProgram() {
	// Create the shader program
	int retval;
	nProgram = glCreateProgram();
	glAttachShader(nProgram, nVertShader);
	glAttachShader(nProgram, nFragShader);
	glLinkProgram(nProgram);
	glGetProgramiv(nProgram, GL_LINK_STATUS, &retval);
	if (retval != GL_TRUE) {
		std::cout << " [error] Failed to create shader program" << std::endl;
		char log[512];
		glGetProgramInfoLog(nProgram, 512, 0x0, log);
		std::cout << " [debug]  output log:" << std::endl; // Print compiler output log
		std::cout << log << std::endl;
		return false;
	}
	std::cout << " [debug] nVertShader=" << nVertShader << std::endl;
	std::cout << " [debug] nFragShader=" << nFragShader << std::endl;
	std::cout << " [debug] nProgram=" << nProgram << std::endl;

	return true;
}

void Shader::setBool(const std::string& name, const bool& value) const {
	glUniform1i(glGetUniformLocation(nProgram, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, const int& value) const {
	glUniform1i(glGetUniformLocation(nProgram, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, const float& value) const {
	glUniform1f(glGetUniformLocation(nProgram, name.c_str()), value);
}

void Shader::setVector2(const std::string& name, const Vector2* vec) const {
	glUniform2fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void Shader::setVector2(const std::string& name, const float& x, const float& y) const {
	glUniform2f(glGetUniformLocation(nProgram, name.c_str()), x, y);
}

void Shader::setVector3(const std::string& name, const Vector3* vec) const {
	glUniform3fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void Shader::setVector3(const std::string& name, const float& x, const float& y, const float& z) const {
	glUniform3f(glGetUniformLocation(nProgram, name.c_str()), x, y, z);
}

void Shader::setVector4(const std::string& name, const Vector4* vec) const {
	glUniform4fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void Shader::setVector4(const std::string& name, const float& x, const float& y, const float& z, const float& w) {
	glUniform4f(glGetUniformLocation(nProgram, name.c_str()), x, y, z, w);
}

void Shader::setMatrix2(const std::string& name, const Matrix2* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void Shader::setMatrix2(const std::string& name, const float* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void Shader::setMatrix3(const std::string& name, const Matrix3* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void Shader::setMatrix3(const std::string& name, const float* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void Shader::setMatrix4(const std::string& name, const Matrix4* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void Shader::setMatrix4(const std::string& name, const float* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

bool Shader::readShader(const std::string& fname, std::string& retval) {
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

bool Shader::compileShader(const unsigned int& nShader, const std::string& sBody) {
	int retval;
	const char* cptr = sBody.data();
	int nLength = (int)sBody.length();
	glShaderSource(nShader, 1, &cptr, &nLength);
	glCompileShader(nShader);
	glGetShaderiv(nShader, GL_COMPILE_STATUS, &retval);
	if (retval != GL_TRUE) {
		std::cout << " [error] Failed to compile shader" << std::endl;
		char log[512];
		glGetShaderInfoLog(nVertShader, 512, 0x0, log);
		std::cout << " [debug]  output log:" << std::endl; // Print compiler output log
		std::cout << log << std::endl;
		return false;
	}
	return true;
}

DefaultShaders::DefaultShader::DefaultShader(const ShaderType& stype_) :
	Shader(),
	good(false),
	type(stype_),
	name("none")
{
	switch (stype_) {
	case ShaderType::NONE:
		break;
	case ShaderType::DEFAULT:
		name = "default";
		good = generate(vertexDefault, vertexDefaultLength, fragmentDefault, fragmentDefaultLength);
		break;
	case ShaderType::MVP:
		name = "mvp";
		good = generate(vertexMVP, vertexMVPLength, fragmentMVP, fragmentMVPLength);
		break;
	case ShaderType::NORMAL:
		name = "normal";
		good = generate(vertexNormal, vertexNormalLength, fragmentNormal, fragmentNormalLength);
		break;
	case ShaderType::ZDEPTH:
		name = "zdepth";
		good = generate(vertexZDepth, vertexZDepthLength, fragmentZDepth, fragmentZDepthLength);
		break;
	default:
		break;
	}
}

bool DefaultShaders::DefaultShader::generate(const std::string* vert, const size_t& nLinesVert, const std::string* frag, const size_t& nLinesFrag) {
	// Create the vertex shader
	std::string vertBody;
	for (size_t i = 0; i < nLinesVert; i++)
		vertBody += vert[i];
	nVertShader = glCreateShader(GL_VERTEX_SHADER);
	if (!compileShader(nVertShader, vertBody)) {
		std::cout << " [debug] Failed to generate vertex shader for default shader name=" << name << std::endl;
		return false;
	}

	// Create the fragment shader
	std::string fragBody;
	for (size_t i = 0; i < nLinesFrag; i++)
		fragBody += frag[i];
	nFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!compileShader(nFragShader, fragBody)) {
		std::cout << " [debug] Failed to generate fragment shader for default shader name=" << name << std::endl;
		return false;
	}

	// Generate the OpenGL shader program
	return generateProgram();
}

const size_t DefaultShaders::vertexDefaultLength = 5;
const size_t DefaultShaders::fragmentDefaultLength = 4;

const std::string DefaultShaders::vertexDefault[vertexDefaultLength] = {
	"void main()\n",
	"{\n",
	"	gl_FrontColor = gl_Color;\n",
	"	gl_Position = ftransform();\n",
	"}\n"
};

const std::string DefaultShaders::fragmentDefault[fragmentDefaultLength] = {
	"void main()\n",
	"{\n",
	"	gl_FragColor = gl_Color;\n",
	"}\n"
};

const size_t DefaultShaders::vertexMVPLength = 8;
const size_t DefaultShaders::fragmentMVPLength = 4;

const std::string DefaultShaders::vertexMVP[vertexMVPLength] = {
	"#version 430 core\n",
	"layout (location = 0) in vec3 position;\n",
	"uniform mat4 MVP;\n",
	"void main()\n",
	"{\n",
	"	gl_FrontColor = gl_Color;\n",
	"	gl_Position = MVP * vec4(position, 1.f);\n",
	"}\n"
};

const std::string DefaultShaders::fragmentMVP[fragmentMVPLength] = {
	"void main()\n",
	"{\n",
	"	gl_FragColor = gl_Color;\n",
	"}\n"
};

const size_t DefaultShaders::vertexNormalLength = 5;
const size_t DefaultShaders::fragmentNormalLength = 4;

const std::string DefaultShaders::vertexNormal[vertexNormalLength] = {
	"varying vec3 vertex_color;\n",
	"void main() {\n",
	"	gl_Position = ftransform(); // Transform coordinates to clip space\n",
	"	vertex_color = vec3(gl_Normal);\n",
	"}\n"
};

const std::string DefaultShaders::fragmentNormal[fragmentNormalLength] = {
	"varying vec3 vertex_color;\n",
	"void main() {\n",
	"	gl_FragColor = vec4(vertex_color, 1.0); // Example of how to use a constructor.\n",
	"}\n"
};

const size_t DefaultShaders::vertexZDepthLength = 3;
const size_t DefaultShaders::fragmentZDepthLength = 12;

const std::string DefaultShaders::vertexZDepth[vertexZDepthLength] = {
	"void main() {\n",
	"	gl_Position = ftransform();\n",
	"}\n"
};

const std::string DefaultShaders::fragmentZDepth[fragmentZDepthLength] = {
	"float near = 0.1;\n",
	"float far = 10.0;\n",
	"float LinearizeDepth(float depth)\n",
	"{\n",
	"	float z = depth * 2.0 - 1.0;\n",
	"	return (2.0 * near * far) / (far + near - z * (far - near));\n",
	"}\n",
	"void main()\n",
	"{\n",
	"	float depth = LinearizeDepth(gl_FragCoord.z) / far;\n",
	"	gl_FragColor = vec4(vec3(depth), 1.0);\n",
	"}\n"
};