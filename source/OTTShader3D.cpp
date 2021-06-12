#include <iostream>
#include <GL/glew.h>

#include "object.hpp"
#include "OTTShader3D.hpp"
#include "OTTTexture.hpp"

void OTTShader3D::enableObjectShader(const object* obj) const {
	(*enableFunc)(const_cast<OTTShader3D*>(this), obj);
	onUserShaderEnable(obj);
}

void OTTShader3D::disableObjectShader(const object* obj) const {
	(*disableFunc)(const_cast<OTTShader3D*>(this), obj);
	onUserShaderDisable(obj);
}

void OTTShader3D::defaultShaderEnable(OTTShader*, const object*) {
}

void OTTShader3D::defaultShaderDisable(OTTShader*, const object*) {
}

void OTTShader3D::bindObjectTexture(OTTShader*, const object* obj) {
	// Bind object texture (if available)
	if (obj->getTexture())
		glBindTexture(GL_TEXTURE_2D, obj->getTexture()->getContext());
}

void OTTShader3D::unbindObjectTexture(OTTShader*, const object*) {
	// Unbind OpenGL texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

OTTDefaultShaders::DefaultShader::DefaultShader(const ShaderType& stype_) :
	OTTShader3D(),
	good(false),
	type(stype_),
	name("none")
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

