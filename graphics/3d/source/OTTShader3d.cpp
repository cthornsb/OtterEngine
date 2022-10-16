#include "OTTShader3d.hpp"

#include <graphics/geometry/OTTObject.hpp>
#include <graphics/texture/OTTTexture.hpp>

#include <iostream>
#include <GL/glew.h>

ott::shaders::DefaultShader::DefaultShader(const ShaderType& stype_) :
	Shader(),
	good(false),
	type(stype_),
	name("none")
{
	switch (stype_) {
	case ShaderType::None:
		break;
	case ShaderType::Simple:
		name = "simple";
		good = this->Generate(vertexSimple, fragmentSimple);
		break;
	case ShaderType::Default:
		name = "default";
		good = this->Generate(vertexDefault, fragmentDefault);
		break;
	case ShaderType::Color:
		name = "color";
		good = this->Generate(vertexColor, fragmentColor);
		break;
	case ShaderType::Normal:
		name = "normal";
		good = this->Generate(vertexNormal, fragmentNormal);
		break;
	case ShaderType::ZDepth:
		name = "zdepth";
		good = this->Generate(vertexZDepth, fragmentZDepth);
		break;
	case ShaderType::Texture:
		name = "texture";
		good = this->Generate(vertexTexture, fragmentTexture);
		enableFunc = &Shader::DefaultShaderEnable;
		disableFunc = &Shader::DefaultShaderDisable;
		break;
	default:
		break;
	}
}

bool ott::shaders::DefaultShader::Generate(const std::vector<std::string>& vert, const std::vector<std::string>& frag) {
	// Create the vertex shader
	std::string vertBody;
	for (auto line = vert.cbegin(); line != vert.cend(); line++)
		vertBody += *line;
	nVertShader = glCreateShader(GL_VERTEX_SHADER);
	if (this->CompileShader(nVertShader, vertBody) == false) {
		std::cout << " [DefaultShaders] Error! Failed to generate vertex shader for default shader name=" << name << std::endl;
		return false;
	}

	// Create the fragment shader
	std::string fragBody;
	for (auto line = frag.cbegin(); line != frag.cend(); line++)
		fragBody += *line;
	nFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (this->CompileShader(nFragShader, fragBody) == false) {
		std::cout << " [DefaultShaders] Error! Failed to generate fragment shader for default shader name=" << name << std::endl;
		return false;
	}

	// Generate the OpenGL shader program
	return this->GenerateProgram();
}

const std::vector<std::string> ott::shaders::vertexSimple = {
	"void main() {\n",
	"	gl_Position = ftransform();\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::fragmentSimple = {
	"void main() {\n",
	"	gl_FragColor = vec4(1,1,1,1);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::vertexDefault = {
	"in vec3 vPosition;\n",
	"uniform mat4 MVP;\n",
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	vertexColor = gl_Color;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::fragmentDefault = {
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	gl_FragColor = vec4(vertexColor, 1.f);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::vertexColor = {
	"in vec3 vPosition;\n",
	"in vec3 vColor;\n",
	"uniform mat4 MVP;\n",
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	vertexColor = vColor;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::fragmentColor = {
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	gl_FragColor = vec4(vertexColor, 1.f);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::vertexNormal = {
	"in vec3 vPosition;\n",
	"in vec3 vNormal;\n",
	"uniform mat4 MVP;\n",
	"varying vec3 vertexColor;",
	"void main() {\n",
	"	vertexColor = vNormal;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::fragmentNormal = {
	"varying vec3 vertexColor;\n",
	"void main() {\n",
	"	gl_FragColor = vec4(vertexColor, 1.0);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::vertexZDepth = {
	"in vec3 vPosition;\n",
	"uniform mat4 MVP;\n",
	"void main() {\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::fragmentZDepth = {
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

const std::vector<std::string> ott::shaders::vertexTexture = {
	"in vec3 vPosition;\n",
	"in vec2 vTexture;\n",
	"varying vec2 uvCoords;",
	"uniform mat4 MVP;\n",
	"void main() {\n",
	"	uvCoords = vTexture;\n",
	"	gl_Position = MVP * vec4(vPosition, 1.f);\n",
	"}\n"
};

const std::vector<std::string> ott::shaders::fragmentTexture = {
	"varying vec2 uvCoords;\n",
	"uniform sampler2D sampler;\n",
	"void main() {\n",
	"	vec4 sampleColor = texture(sampler, uvCoords);\n",
	"	if (sampleColor.a == 0.f)\n",
	"		discard;\n",
	"	gl_FragColor = sampleColor;\n",
	"}\n"
};

