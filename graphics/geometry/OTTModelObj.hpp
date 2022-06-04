#ifndef GRAPHICS_GEOMETRY_OTT_MODEL_OBJ_HPP
#define GRAPHICS_GEOMETRY_OTT_MODEL_OBJ_HPP

#include "OTTModel.hpp"

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace ott {

class Object;
class Texture;

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

	ModelObj(const std::string& fname, const UnitType& unit = UnitType::Inch) :
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
		this->Read(fname);
	}

	~ModelObj();

	void OnUserBuild() override ;

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

		std::unique_ptr<Texture> ambientTextureMap;
		std::unique_ptr<Texture> diffuseTextureMap;
		std::unique_ptr<Texture> specularTextureMap;

		uint32_t nAssociatedFaces;

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

	uint32_t vertexCount;
	uint32_t faceCount;
	uint32_t quadCount;
	uint32_t triCount;
	uint32_t vertNormCount;
	uint32_t vertTextureCount;

	std::string sName;

	std::vector<std::unique_ptr<Material> > materials;

	std::map<std::string, std::unique_ptr<Object> > childObjects;

	uint32_t OnUserRead(std::ifstream* f) override ;

	bool ReadMaterialFile(const std::string& filename);

	Material* FindMaterial(const std::string& name);
};

} /* namespace ott */

#endif // #ifndef GRAPHICS_GEOMETRY_OTT_MODEL_OBJ_HPP
