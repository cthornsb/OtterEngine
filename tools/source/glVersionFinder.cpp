#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "VersionNumber.hpp"

// Set if a fatal error occured
bool bHadError = false;

void handleErrorsGLFW(int32_t error, const char* description) {
	std::cout << " [glfw] Error! id=" << error << " : " << description << std::endl;
	bHadError = true;
}

int32_t main(int32_t argc, char* argv[]) {
	std::string outputFilename;
	if (argc >= 2) { // User set output directory
		outputFilename = std::string(argv[1]);
	}
	else {
		outputFilename = ".";
	}

	// Set output filename
	outputFilename += "/OTTVersions.hpp";

	// Handle detailed GLFW errors
	glfwSetErrorCallback(handleErrorsGLFW);

	// Initialize GLFW
	int32_t glfwRetval = glfwInit();
	if (glfwRetval == GLFW_FALSE) { // GLFW initialization error
		std::cout << " [glVersionFinder] Encountered fatal error!" << std::endl;
		return 1;
	}

	// Create an OpenGL context but do not make the window visible
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* win = glfwCreateWindow(256, 256, "", 0, 0);

	// Initialize GLEW
	glfwMakeContextCurrent(win);
	GLint err = glewInit();
	if (err != GLEW_OK) {
		const GLubyte* str = glewGetErrorString(err);
		std::cout << " [glew] Error! id=" << err << " : " << str << std::endl;
		std::cout << " [glVersionFinder] Encountered fatal error!" << std::endl;
		return 2;
	}

	// Get OpenGL version
	std::string stringVersionOpenGL = std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))); // Since OpenGL 2.0
	ott::VersionNumber numericalVersionOpenGL(stringVersionOpenGL);

	// Get OpenGL Shader Language (GLSL) version
	std::string stringVersionGLSL = std::string(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))); // Since OpenGL 2.0
	ott::VersionNumber numericalVersionGLSL(stringVersionGLSL);

	// Get GLEW version
	std::string stringVersionGLEW = std::string(reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));
	ott::VersionNumber numericalVersionGLEW(stringVersionGLEW);

	// Get GLFW version
	ott::VersionNumber numericalVersionGLFW(GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

	// Print version strings
	std::cout << " [opengl] GL Version: " << numericalVersionOpenGL.getString() << " (" << stringVersionOpenGL << ")" << std::endl;
	std::cout << " [opengl] GLSL Version: " << numericalVersionGLSL.getString() << " (" << stringVersionGLSL << ")" << std::endl;
	std::cout << " [glew] GLEW Version: " << numericalVersionGLEW.getString() << " (" << stringVersionGLEW << ")" << std::endl;
	std::cout << " [glfw] GLFW Version: " << numericalVersionGLFW.getString() << std::endl;

	std::ofstream ofile(outputFilename.c_str());
	if (!ofile.good()) { // Failed to open output file
		std::cout << " [glVersionFinder] Failed to open output file \"" << outputFilename << "\"" << std::endl;
		std::cout << " [glVersionFinder] Encountered fatal error!" << std::endl;
		return 3;
	}

	// Close OpenGL context
	glfwDestroyWindow(win);

	// Write version strings to file
	ofile << "#ifndef OTTER_VERSION_HPP" << std::endl;
	ofile << "#define OTTER_VERSION_HPP" << std::endl;
	ofile << std::endl;
	ofile << "#define OTTER_VERSION_STRING \"" << "N/A" << "\"" << std::endl;
	ofile << "#define OTTER_VERSION_NUMBER 0.0" << std::endl;
	ofile << "#define OTTER_VERSION_MAJOR 0" << std::endl;
	ofile << "#define OTTER_VERSION_MINOR 0" << std::endl;
	ofile << std::endl;
	ofile << "#define OPENGL_VERSION_STRING \"" << numericalVersionOpenGL.getString() << "\"" << std::endl;
	ofile << "#define OPENGL_VERSION_NUMBER " << numericalVersionOpenGL.getMajorVersion() << "." << numericalVersionOpenGL.getMinorVersion() << std::endl;
	ofile << "#define OPENGL_VERSION_MAJOR " << numericalVersionOpenGL.getMajorVersion() << std::endl;
	ofile << "#define OPENGL_VERSION_MINOR " << numericalVersionOpenGL.getMinorVersion() << std::endl;
	ofile << std::endl;
	ofile << "#define GLSL_VERSION_STRING \"" << numericalVersionGLSL.getString() << "\"" << std::endl;
	ofile << "#define GLSL_VERSION_NUMBER " << numericalVersionGLSL.getMajorVersion() << "." << numericalVersionGLSL.getMinorVersion() << std::endl;
	ofile << "#define GLSL_VERSION_MAJOR " << numericalVersionGLSL.getMajorVersion() << std::endl;
	ofile << "#define GLSL_VERSION_MINOR " << numericalVersionGLSL.getMinorVersion() << std::endl;
	ofile << std::endl;
	ofile << "#define GLEW_VERSION_STRING \"" << numericalVersionGLEW.getString() << "\"" << std::endl;
	ofile << "#define GLEW_VERSION_NUMBER " << numericalVersionGLEW.getMajorVersion() << "." << numericalVersionGLEW.getMinorVersion() << std::endl;
	//ofile << "#define GLEW_VERSION_MAJOR " << numericalVersionGLEW.getMajorVersion() << std::endl; // Already defined by GLEW
	//ofile << "#define GLEW_VERSION_MINOR " << numericalVersionGLEW.getMinorVersion() << std::endl; // Already defined by GLEW
	ofile << std::endl;
	ofile << "#define GLFW_VERSION_STRING \"" << numericalVersionGLFW.getString() << "\"" << std::endl;
	ofile << "#define GLFW_VERSION_NUMBER " << numericalVersionGLFW.getMajorVersion() << "." << numericalVersionGLFW.getMinorVersion() << std::endl;
	//ofile << "#define GLFW_VERSION_MAJOR " << numericalVersionGLFW.getMajorVersion() << std::endl; // Already defined by GLFW
	//ofile << "#define GLFW_VERSION_MINOR " << numericalVersionGLFW.getMinorVersion() << std::endl; // Already defined by GLFW
	ofile << std::endl;
	ofile << "#endif // ifndef OTTER_VERSION_HPP" << std::endl;
	ofile << std::endl;
	ofile.close();

	std::cout << " [glVersionFinder] Successfully wrote output file header." << std::endl;

	return 0; // success
}