#ifndef MODEL_OBJ_HPP
#define MODEL_OBJ_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "Model.hpp"
#include "Vector.hpp"

class OTTTexture;

enum class OBJ_TYPE {
	NONE,
	MTLLIB,
	USEMTL,
	OBJECT,
	GROUP,
	SMOOTH,
	VERTEX,
	NORMAL,
	TEXTURE,
	FACE
};

class ModelObj : public Model {
public:
	ModelObj() :
		Model("obj"),
		objectName(),
		groupName(),
		vertexCount(0),
		faceCount(0),
		quadCount(0),
		triCount(0),
		vertNormCount(0),
		vertTextureCount(0)
	{
	}

	ModelObj(const std::string& fname, const UNITS& unit = UNITS::INCH) :
		Model("obj", unit),
		objectName(),
		groupName(),
		vertexCount(0),
		faceCount(0),
		quadCount(0),
		triCount(0),
		vertNormCount(0),
		vertTextureCount(0)
	{
		read(fname);
	}

	~ModelObj();

	virtual void userBuild();

protected:
	class Material {
	public:
		std::string sName;

		ColorRGB ambient;
		ColorRGB diffuse;
		ColorRGB specular;

		float specularRange;
		float indexOfRefraction;
		float opacity;

		std::unique_ptr<OTTTexture> ambientTextureMap;
		std::unique_ptr<OTTTexture> diffuseTextureMap;
		std::unique_ptr<OTTTexture> specularTextureMap;

		unsigned int nAssociatedFaces;

		Material() :
			sName("unnamed"),
			ambient(1.f, 1.f, 1.f),
			diffuse(1.f, 1.f, 1.f),
			specular(1.f, 1.f, 1.f),
			specularRange(0.f),
			indexOfRefraction(1.f),
			opacity(1.f),
			ambientTextureMap(),
			diffuseTextureMap(),
			specularTextureMap(),
			nAssociatedFaces(0)
		{
		}

		Material(const std::string& name) :
			sName(name),
			ambient(1.f, 1.f, 1.f),
			diffuse(1.f, 1.f, 1.f),
			specular(1.f, 1.f, 1.f),
			specularRange(0.f),
			indexOfRefraction(1.f),
			opacity(1.f),
			ambientTextureMap(),
			diffuseTextureMap(),
			specularTextureMap(),
			nAssociatedFaces(0)
		{
		}
	};

	std::string objectName;
	std::string groupName;

	unsigned int vertexCount;
	unsigned int faceCount;
	unsigned int quadCount;
	unsigned int triCount;
	unsigned int vertNormCount;
	unsigned int vertTextureCount;

	std::string sName;

	std::vector<std::unique_ptr<Material> > materials;

	std::map<std::string, std::unique_ptr<SubObject> > childObjects;

	virtual unsigned int userRead(std::ifstream& f);

	bool readLine(std::ifstream& f, OBJ_TYPE& type, std::vector<std::string>& args);

	bool readMaterialFile(const std::string& filename);

	std::string removeCharacter(const std::string& input, const char& ch);

	unsigned int splitString(const std::string& input, std::vector<std::string>& args, const char& delim = ' ');

	Material* findMaterial(const std::string& name);
};

#endif
