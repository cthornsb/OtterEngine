//#include <GL/freeglut.h>
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
	int nLength[1] = { sBody.length() };
	glShaderSource(nShader, 1, &cptr, nLength);
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
}