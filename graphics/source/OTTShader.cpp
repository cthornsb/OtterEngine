#include <iostream>
#include <fstream>
#include <GL/glew.h>

#include "OTTShader.hpp"

OTTShader::~OTTShader() {
	if (nVertShader)
		glDeleteShader(nVertShader);
	if (nGeomShader)
		glDeleteShader(nGeomShader);
	if (nFragShader)
		glDeleteShader(nFragShader);
	glDeleteProgram(nProgram);
}

void OTTShader::enableShader() const {
	glUseProgram(nProgram);
}

void OTTShader::disableShader() const {
	glUseProgram(0);
}

bool OTTShader::generate(const std::string& vert, const std::string& frag) {
	// Create the vertex shader
	if (!buildVertexShader(vert))
		return false;

	// Create the fragment shader
	if (!buildFragmentShader(frag))
		return false;

	// Generate the OpenGL shader program
	return generateProgram();
}

bool OTTShader::generate(const std::string& vert, const std::string& geom, const std::string& frag) {
	// Create the vertex shader
	if (!buildVertexShader(vert))
		return false;

	// Create the geometry shader
	if (!buildGeometryShader(geom))
		return false;

	// Create the fragment shader
	if (!buildFragmentShader(frag))
		return false;

	// Generate the OpenGL shader program
	return generateProgram();
}

bool OTTShader::buildVertexShader(const std::string& fname) {
	std::string sBody;
	if (!readShader(fname, sBody))
		return false;
	nVertShader = glCreateShader(GL_VERTEX_SHADER);
	return compileShader(nVertShader, sBody);
}

bool OTTShader::buildGeometryShader(const std::string& fname) {
	std::string sBody;
	if (!readShader(fname, sBody))
		return false;
	nGeomShader = glCreateShader(GL_GEOMETRY_SHADER);
	return compileShader(nGeomShader, sBody);
}

bool OTTShader::buildFragmentShader(const std::string& fname) {
	std::string sBody;
	if (!readShader(fname, sBody))
		return false;
	nFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	return compileShader(nFragShader, sBody);
}

bool OTTShader::generateProgram() {
	// Create the shader program
	int retval;
	nProgram = glCreateProgram();

	// Bind default attribtue locations (equivalent to "layout (location = N)")
	glBindAttribLocation(nProgram, 0, "vPosition");
	glBindAttribLocation(nProgram, 1, "vNormal");
	glBindAttribLocation(nProgram, 2, "vTexture");

	// Link shader program
	if(nVertShader)
		glAttachShader(nProgram, nVertShader);
	if(nGeomShader)
		glAttachShader(nProgram, nGeomShader);
	if(nFragShader)
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
	if (nVertShader)
		std::cout << " [Shader] debug: nVertShader=" << nVertShader << std::endl;
	if(nGeomShader)
		std::cout << " [Shader] debug: nGeomShader=" << nGeomShader << std::endl;
	if(nFragShader)
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

void OTTShader::setVector2(const std::string& name, const Vector2& vec) const {
	glUniform2fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec.getConstData());
}

void OTTShader::setVector2(const std::string& name, const Vector2* vec) const {
	glUniform2fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void OTTShader::setVector2(const std::string& name, const float& x, const float& y) const {
	glUniform2f(glGetUniformLocation(nProgram, name.c_str()), x, y);
}

void OTTShader::setVector3(const std::string& name, const Vector3& vec) const {
	glUniform3fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec.getConstData());
}

void OTTShader::setVector3(const std::string& name, const Vector3* vec) const {
	glUniform3fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void OTTShader::setVector3(const std::string& name, const float& x, const float& y, const float& z) const {
	glUniform3f(glGetUniformLocation(nProgram, name.c_str()), x, y, z);
}

void OTTShader::setVector4(const std::string& name, const Vector4& vec) const {
	glUniform4fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec.getConstData());
}

void OTTShader::setVector4(const std::string& name, const Vector4* vec) const {
	glUniform4fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->getConstData());
}

void OTTShader::setVector4(const std::string& name, const float& x, const float& y, const float& z, const float& w) const {
	glUniform4f(glGetUniformLocation(nProgram, name.c_str()), x, y, z, w);
}

void OTTShader::setMatrix2(const std::string& name, const Matrix2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat.getConstData());
}

void OTTShader::setMatrix2(const std::string& name, const Matrix2* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void OTTShader::setMatrix2(const std::string& name, const float* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void OTTShader::setMatrix3(const std::string& name, const Matrix3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat.getConstData());
}

void OTTShader::setMatrix3(const std::string& name, const Matrix3* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void OTTShader::setMatrix3(const std::string& name, const float* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void OTTShader::setMatrix4(const std::string& name, const Matrix4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat.getConstData());
}

void OTTShader::setMatrix4(const std::string& name, const Matrix4* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->getConstData());
}

void OTTShader::setMatrix4(const std::string& name, const float* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

int OTTShader::getUniformLocation(const std::string& name) const {
	return glGetUniformLocation(nProgram, name.c_str());
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
		glGetShaderInfoLog(nShader, 512, 0x0, log);
		std::cout << " [debug]  output log:" << std::endl; // Print compiler output log
		std::cout << log << std::endl;
		return false;
	}
	return true;
}
