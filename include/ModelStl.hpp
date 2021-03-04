#ifndef MODEL_STL_HPP
#define MODEL_STL_HPP

#include <string>
#include <vector>

#include "Model.hpp"
#include "Vector.hpp"
#include "object.hpp"

class ModelStl : public Model {
public:
	ModelStl();

	ModelStl(const std::string& fname, const UNITS& unit = UNITS::INCH) :
		Model("stl", unit),
		reversed(false)
	{
		read(fname);
	}

	virtual void userBuild();

protected:
	bool reversed;

	virtual unsigned int userRead(std::ifstream&);

	void convertToStandard(Vector3& vec);

	void readStlBlock(float* array);

	bool readAstBlock(const std::vector<std::string>& block);

	bool isBinaryFile(std::ifstream* f);

	unsigned int readAscii(std::ifstream* f);

	unsigned int readBinary(std::ifstream* f);

	Vector3 getVectorFromString(const std::string& str);

	std::string readString(std::ifstream* f, const size_t& nBytes);

	float readFloatFromFile(std::ifstream* f);

	unsigned int readUIntFromFile(std::ifstream* f);

	unsigned short readUShortFromFile(std::ifstream* f);
};

#endif
