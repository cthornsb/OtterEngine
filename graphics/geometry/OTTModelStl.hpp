#ifndef GRAPHICS_GEOMETRY_OTT_MODEL_STL_HPP
#define GRAPHICS_GEOMETRY_OTT_MODEL_STL_HPP

#include "OTTModel.hpp"

#include <string>
#include <vector>

namespace ott {

class ModelStl : public Model {
public:
	ModelStl();

	ModelStl(const std::string& fname, const UnitType& unit = UnitType::Inch) :
		Model("stl", unit),
		reversed(false)
	{
		this->Read(fname);
	}

	void OnUserBuild() override ;

protected:
	bool reversed;

	uint32_t OnUserRead(std::ifstream*) override ;

	void ReadStlBlock(float* array);

	bool ReadAstBlock(const std::vector<std::string>& block);

	bool IsBinaryFile(std::ifstream* f);

	uint32_t ReadAscii(std::ifstream* f);

	uint32_t ReadBinary(std::ifstream* f);

	Vector3 GetVectorFromString(const std::string& str);
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_MODEL_STL_HPP
