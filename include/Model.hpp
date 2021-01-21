#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <fstream>

#include "Vector.hpp"
#include "object.hpp"

enum class UNITS { INCH, FOOT, METER, DECIMETER, CENTIMETER, MILLIMETER, MICROMETER, NANOMETER };

class Model : public object {
public:
	Model() :
		object(),
		path(),
		directory(),
		filename(),
		extension(),
		isGood(false),
		debugMode(false),
		conversion(UNITS::INCH)
	{
	}

	Model(const std::string& ext, const UNITS& unit = UNITS::INCH) :
		object(),
		path(),
		directory(),
		filename(),
		extension(ext),
		isGood(false),
		debugMode(false),
		conversion(unit)
	{
	}

	virtual ~Model() { }

	unsigned int read(const std::string& fname);

	virtual void userBuild() { }

protected:
	std::string path;
	std::string directory;
	std::string filename;

	std::string extension;

	bool isGood;

	bool debugMode;

	UNITS conversion;

	virtual unsigned int userRead(std::ifstream&) {
		return 0;
	}

	std::string getHex8(const unsigned char& input);

	std::string getHex16(const unsigned short& input);

	void readFromFile(std::ifstream& f, unsigned char& val);

	void readFromFile(std::ifstream& f, unsigned short& val);

	void readFromFile(std::ifstream& f, unsigned int& val);

	void readFromFile(std::ifstream& f, float& val);

	void convertToStandard(Vector3& vec);
};

#endif