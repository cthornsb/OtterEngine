#ifndef GRAPHICS_GEOMETRY_OTT_MODEL_HPP
#define GRAPHICS_GEOMETRY_OTT_MODEL_HPP

#include "OTTObject.hpp"

#include <math/OTTVector.hpp>

#include <string>
#include <fstream>

namespace ott {

enum class UnitType { 
	Inch, 
	Foot, 
	Meter, 
	Decimeter,
	Centimeter, 
	Millimeter, 
	Micrometer, 
	Nanometer 
};

class Model : public Object {
public:
	Model() :
		Object(),
		path(),
		directory(),
		filename(),
		extension(),
		isGood(false),
		debugMode(false),
		conversion(UnitType::Inch)
	{
	}

	Model(const std::string& ext, const UnitType& unit = UnitType::Inch) :
		Object(),
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

	uint32_t Read(const std::string& fname);

protected:
	std::string path;
	std::string directory;
	std::string filename;

	std::string extension;

	bool isGood;

	bool debugMode;

	UnitType conversion;

	virtual uint32_t OnUserRead(std::ifstream*) {
		return 0;
	}

	void ConvertToStandard(Vector3& vec);

	virtual void OnUserBuild() { }

	static std::string Hex8(const uint8_t& input);

	static std::string Hex16(const uint16_t& input);

	static uint8_t ReadUInt8(std::ifstream* f);

	static uint16_t ReadUInt16(std::ifstream* f);

	static uint32_t ReadUInt32(std::ifstream* f);

	static float ReadFloat(std::ifstream* f);

	static std::string ReadString(std::ifstream* f, const size_t& nBytes);
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_MODEL_HPP
