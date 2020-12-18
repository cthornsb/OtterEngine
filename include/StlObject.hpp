#ifndef STL_OBJECT_HPP
#define STL_OBJECT_HPP

#include <string>
#include <vector>

#include "vector3.hpp"
#include "object.hpp"

enum class UNITS { INCH, FOOT, METER, DECIMETER, CENTIMETER, MILLIMETER, MICROMETER, NANOMETER };

class StlObject : public object {
public:
	StlObject() :
		object(),
		isGood(false),
		reversed(false),
		conversion(UNITS::INCH)
	{
	}

	StlObject(const std::string& fname, const UNITS& unit = UNITS::INCH) :
		StlObject()
	{
		read(fname);
	}

	unsigned int read(const std::string& fname, const UNITS& unit = UNITS::INCH);

	virtual void userBuild();

private:
	bool isGood;
	bool reversed;

	UNITS conversion;

	std::string names[3];

	void convertToStandard(vector3& vec);

	void readStlBlock(float* array);

	bool readAstBlock(const std::vector<std::string>& block);

	bool isBinaryFile(std::ifstream* f);

	unsigned int readAscii(std::ifstream* f);

	unsigned int readBinary(std::ifstream* f);

	vector3 getVectorFromString(const std::string& str);

	std::string getHex(const unsigned char& input);

	std::string readString(std::ifstream* f, const size_t& nBytes);

	float readFloatFromFile(std::ifstream* f);

	unsigned int readUIntFromFile(std::ifstream* f);

	unsigned short readUShortFromFile(std::ifstream* f);
};

#endif