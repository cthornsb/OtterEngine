#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

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

	unsigned int getProgram() const { return nProgram; }

	bool generate(const std::string& vert, const std::string& frag);

private:
	unsigned int nVertShader; ///< OpenGl shader context ID
	unsigned int nFragShader; ///< OpenGl shader context ID

	unsigned int nProgram; ///< OpenGl program ID

	bool readShader(const std::string& fname, std::string& retval);

	bool compileShader(const unsigned int& nShader, const std::string& sBody);
};

#endif