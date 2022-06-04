#include "OTTShader.hpp"

#include <math/OTTVector.hpp>
#include <math/OTTMatrix.hpp>

#include <iostream>
#include <fstream>
#include <GL/glew.h>

constexpr uint16_t kLogBufferSizeBytes = 512;

ott::Shader::~Shader() {
	if (nVertShader)
		glDeleteShader(nVertShader);
	if (nGeomShader)
		glDeleteShader(nGeomShader);
	if (nFragShader)
		glDeleteShader(nFragShader);
	glDeleteProgram(nProgram);
}

void ott::Shader::EnableShader() const {
	glUseProgram(nProgram);
}

void ott::Shader::DisableShader() const {
	glUseProgram(0);
}

bool ott::Shader::Generate(const std::string& vert, const std::string& frag) {
	// Create the vertex shader
	if (this->BuildVertexShader(vert) == false) {
		return false;
	}

	// Create the fragment shader
	if (this->BuildFragmentShader(frag) == false) {
		return false;
	}

	// Generate the OpenGL shader program
	return this->GenerateProgram();
}

bool ott::Shader::Generate(const std::string& vert, const std::string& geom, const std::string& frag) {
	// Create the vertex shader
	if (this->BuildVertexShader(vert) == false) {
		return false;
	}

	// Create the geometry shader
	if (this->BuildGeometryShader(geom) == false) {
		return false;
	}

	// Create the fragment shader
	if (this->BuildFragmentShader(frag) == false) {
		return false;
	}

	// Generate the OpenGL shader program
	return this->GenerateProgram();
}

bool ott::Shader::BuildVertexShader(const std::string& fname) {
	std::string sBody;
	if (this->ReadShader(fname, sBody) == false) {
		return false;
	}
	nVertShader = glCreateShader(GL_VERTEX_SHADER);
	return this->CompileShader(nVertShader, sBody);
}

bool ott::Shader::BuildGeometryShader(const std::string& fname) {
	std::string sBody;
	if (this->ReadShader(fname, sBody) == false) {
		return false;
	}
	nGeomShader = glCreateShader(GL_GEOMETRY_SHADER);
	return this->CompileShader(nGeomShader, sBody);
}

bool ott::Shader::BuildFragmentShader(const std::string& fname) {
	std::string sBody;
	if (this->ReadShader(fname, sBody) == false) {
		return false;
	}
	nFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	return this->CompileShader(nFragShader, sBody);
}

bool ott::Shader::GenerateProgram() {
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
		char log[kLogBufferSizeBytes];
		glGetProgramInfoLog(nProgram, kLogBufferSizeBytes, 0x0, log);
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

void ott::Shader::SetBool(const std::string& name, const bool& value) const {
	glUniform1i(glGetUniformLocation(nProgram, name.c_str()), (int)value);
}

void ott::Shader::SetInt(const std::string& name, const int& value) const {
	glUniform1i(glGetUniformLocation(nProgram, name.c_str()), value);
}

void ott::Shader::SetFloat(const std::string& name, const float& value) const {
	glUniform1f(glGetUniformLocation(nProgram, name.c_str()), value);
}

void ott::Shader::SetVector2(const std::string& name, const Vector2& vec) const {
	glUniform2fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec.ConstData());
}

void ott::Shader::SetVector2(const std::string& name, const Vector2* vec) const {
	glUniform2fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->ConstData());
}

void ott::Shader::SetVector2(const std::string& name, const float& x, const float& y) const {
	glUniform2f(glGetUniformLocation(nProgram, name.c_str()), x, y);
}

void ott::Shader::SetVector3(const std::string& name, const Vector3& vec) const {
	glUniform3fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec.ConstData());
}

void ott::Shader::SetVector3(const std::string& name, const Vector3* vec) const {
	glUniform3fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->ConstData());
}

void ott::Shader::SetVector3(const std::string& name, const float& x, const float& y, const float& z) const {
	glUniform3f(glGetUniformLocation(nProgram, name.c_str()), x, y, z);
}

void ott::Shader::SetVector4(const std::string& name, const Vector4& vec) const {
	glUniform4fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec.ConstData());
}

void ott::Shader::SetVector4(const std::string& name, const Vector4* vec) const {
	glUniform4fv(glGetUniformLocation(nProgram, name.c_str()), 1, vec->ConstData());
}

void ott::Shader::SetVector4(const std::string& name, const float& x, const float& y, const float& z, const float& w) const {
	glUniform4f(glGetUniformLocation(nProgram, name.c_str()), x, y, z, w);
}

void ott::Shader::SetMatrix2(const std::string& name, const Matrix2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat.ConstData());
}

void ott::Shader::SetMatrix2(const std::string& name, const Matrix2* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->ConstData());
}

void ott::Shader::SetMatrix2(const std::string& name, const float* mat) const {
	glUniformMatrix2fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void ott::Shader::SetMatrix3(const std::string& name, const Matrix3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat.ConstData());
}

void ott::Shader::SetMatrix3(const std::string& name, const Matrix3* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->ConstData());
}

void ott::Shader::SetMatrix3(const std::string& name, const float* mat) const {
	glUniformMatrix3fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

void ott::Shader::SetMatrix4(const std::string& name, const Matrix4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat.ConstData());
}

void ott::Shader::SetMatrix4(const std::string& name, const Matrix4* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat->ConstData());
}

void ott::Shader::SetMatrix4(const std::string& name, const float* mat) const {
	glUniformMatrix4fv(glGetUniformLocation(nProgram, name.c_str()), 1, GL_FALSE, mat);
}

int ott::Shader::GetUniformLocation(const std::string& name) const {
	return glGetUniformLocation(nProgram, name.c_str());
}

bool ott::Shader::ReadShader(const std::string& fname, std::string& retval) {
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

bool ott::Shader::CompileShader(const unsigned int& nShader, const std::string& sBody) {
	int retval;
	const char* cptr = sBody.data();
	int nLength = (int)sBody.length();
	glShaderSource(nShader, 1, &cptr, &nLength);
	glCompileShader(nShader);
	glGetShaderiv(nShader, GL_COMPILE_STATUS, &retval);
	if (retval != GL_TRUE) {
		std::cout << " [Shader] Error! Failed to compile shader" << std::endl;
		char log[kLogBufferSizeBytes];
		glGetShaderInfoLog(nShader, kLogBufferSizeBytes, 0x0, log);
		std::cout << " [debug]  output log:" << std::endl; // Print compiler output log
		std::cout << log << std::endl;
		return false;
	}
	return true;
}
