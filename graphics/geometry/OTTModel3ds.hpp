#ifndef GRAPHICS_GEOMETRY_OTT_MODEL_3DS_HPP
#define GRAPHICS_GEOMETRY_OTT_MODEL_3DS_HPP

#include "OTTModel.hpp"

#include <string>
#include <vector>

namespace ott {

class Model3ds : public Model {
public:
	Model3ds() :
		Model("3ds")
	{
	}

	Model3ds(const std::string& fname, const UnitType& unit = UnitType::Inch) :
		Model("3ds", unit)
	{
		this->Read(fname);
	}

	void OnUserBuild() override ;

protected:
	class Chunk {
	public:
		uint16_t nChunkID;

		uint32_t nChunkBytes;

		uint32_t nBytesLeftInChunk;

		Chunk() :
			nChunkID(0),
			nChunkBytes(0),
			nBytesLeftInChunk(0)
		{
		}

		Chunk(std::ifstream* f) :
			Chunk()
		{
			f->read((char*)&nChunkID, 2);
			f->read((char*)&nChunkBytes, 4);
			nChunkBytes -= 6; // Skip the chunk header
			nBytesLeftInChunk = nChunkBytes;
		}

		std::string ReadAsciiZ(std::ifstream* f);

		void SkipToNextChunk(std::ifstream* f);
	};

	class Material {
	public:
		std::string sName;

		Material() :
			sName()
		{
		}

		Material(const std::string& name) :
			sName(name)
		{
		}
	};

	Vector3 vertexPosition;
	Vector2 textureCoords;

	float fScale;
	float fMatrix;

	uint32_t nVersion;
	uint32_t nMeshVersion;

	uint16_t polyIndicies[4];
	uint16_t nVertices;
	uint16_t nPoints;

	std::string sName;

	std::vector<Material> materials;

	uint32_t OnUserRead(std::ifstream* f) override ;

	bool ReadChunk(std::ifstream* f);

	const Material* FindMaterial(const std::string& name) const;
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_MODEL_3DS_HPP
