#ifndef MODEL_3DS_HPP
#define MODEL_3DS_HPP

#include <string>
#include <vector>

#include "Model.hpp"
#include "Vector.hpp"

class Model3ds : public Model {
public:
	Model3ds() :
		Model("3ds")
	{
	}

	Model3ds(const std::string& fname, const UNITS& unit = UNITS::INCH) :
		Model("3ds", unit)
	{
		read(fname);
	}

	virtual void userBuild();

protected:
	class Chunk {
	public:
		unsigned short nChunkID;

		unsigned int nChunkBytes;

		unsigned int nBytesLeftInChunk;

		Chunk() :
			nChunkID(0),
			nChunkBytes(0),
			nBytesLeftInChunk(0)
		{
		}

		Chunk(std::ifstream& f) :
			Chunk()
		{
			f.read((char*)&nChunkID, 2);
			f.read((char*)&nChunkBytes, 4);
			nChunkBytes -= 6; // Skip the chunk header
			nBytesLeftInChunk = nChunkBytes;
		}

		std::string getAsciiZ(std::ifstream& f);

		void skipToNextChunk(std::ifstream& f);
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

	unsigned int nVersion;
	unsigned int nMeshVersion;

	unsigned short polyIndicies[4];
	unsigned short nVertices;
	unsigned short nPoints;

	std::string sName;

	std::vector<Material> materials;

	virtual unsigned int userRead(std::ifstream& f);

	bool readChunk(std::ifstream& f);

	const Material* findMaterial(const std::string& name) const;
};

#endif