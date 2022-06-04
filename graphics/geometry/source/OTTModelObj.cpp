#include "OTTModelObj.hpp"

#include <core/OTTStringOps.hpp>
#include <graphics/core/OTTColor.hpp>
#include <graphics/texture/OTTTexture.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>

namespace ott {

enum class ObjType {
	None,
	MtlLib,
	UseMtl,
	Object,
	Group,
	Smooth,
	Vertex,
	Normal,
	Texture,
	Face
};

bool ReadLine(std::ifstream* f, ObjType& type, std::vector<std::string>& args);

} /* namespace obj */

ott::ModelObj::~ModelObj() {
}

void ott::ModelObj::OnUserBuild() {
	// Link material textures to sub-objects
	for (auto mat = materials.cbegin(); mat != materials.cend(); mat++) {
		Material* mptr = mat->get();
		if (!mptr->ambientTextureMap) {
			continue;
		}
		auto child = childObjects.find(mptr->sName);
		if (child == childObjects.end()) {
			continue;
		}
		mptr->ambientTextureMap->GetTexture(); // Convert to OpenGL texture
		if (debugMode) {
			std::cout << " [debug] Converting to OpenGL texture (" << mptr->ambientTextureMap->Context() << ")" << std::endl;
		}
		child->second->SetTexture(mptr->ambientTextureMap.get());
		mptr->ambientTextureMap->Free(); // Free up image memory
	}

	// Finalize sub-objects and add them to the parent object
	for (auto obj = childObjects.cbegin(); obj != childObjects.cend(); obj++) {
		Object* child = obj->second.get();
		if (!child || child->Polygons()->Empty()) { // Skip empty objects
			continue;
		}
		// This call will automatically generate the sub-object's VBO and free up conventional memory
		this->AddChild(child);
	}
}

uint32_t ott::ModelObj::OnUserRead(std::ifstream* f) {
	// Add default material	
	Material* currentMaterial = new Material();
	materials.push_back(std::unique_ptr<Material>(currentMaterial));

	Object* currentChild = new Object();
	childObjects[currentMaterial->sName] = std::unique_ptr<Object>(currentChild);

	std::string asciiString;
	std::vector<std::string> args;
	ObjType cmd;

	// Read the file and count number of vertices and polygons
	uint32_t lineCount = 0;
	while (ott::ReadLine(f, cmd, args)) { // Read model data
		lineCount++;
		switch (cmd) {
		case ObjType::MtlLib:
			asciiString = "";
			for (auto str = args.cbegin() + 1; str != args.cend(); str++) {
				asciiString += *str;
			}
			if (debugMode) {
				std::cout << " [debug] Reading material file \"" << asciiString << "\"" << std::endl;
			}
			this->ReadMaterialFile(directory + "/" + asciiString);
			break;
		case ObjType::UseMtl:
			currentMaterial = this->FindMaterial(args[1]);
			if (currentMaterial) {
				auto obj = childObjects.find(currentMaterial->sName);
				if (obj == childObjects.end()) { // New child object
					currentChild = new Object();
					childObjects[currentMaterial->sName] = std::unique_ptr<Object>(currentChild);
					if (debugMode) {
						std::cout << " [debug] Adding new sub-object associated with material \"" << currentMaterial->sName << "\"" << std::endl;
					}
				}
				else {
					currentChild = obj->second.get();
				}
			}
			else {
				std::cout << " [error] On line " << lineCount << ", undefined material \"" << args[1] << "\"" << std::endl;
			}
			break;
		case ObjType::Object:
			objectName = args[1];
			if (debugMode) {
				std::cout << " [debug] Object Name: " << objectName << std::endl;
			}
			break;
		case ObjType::Group:
			groupName = args[1];
			if (debugMode) {
				std::cout << " [debug] Group Name: " << groupName << std::endl;
			}
			break;
		case ObjType::Smooth:
			break;
		case ObjType::Vertex:
			vertexCount++;
			break;
		case ObjType::Normal:
			vertNormCount++;
			break;
		case ObjType::Texture:
			vertTextureCount++;
			break;
		case ObjType::Face:
			if (args.size() >= 5) { // Quad
				quadCount++;
				currentMaterial->nAssociatedFaces += 2;
			}
			else if (args.size() >= 4) { // Triangle
				triCount++;
				currentMaterial->nAssociatedFaces++;
			}
			break;
		default:
			break;
		}
	}

	if (debugMode) { // Print debug information
		std::cout << " [debug] Defined Vertices = " << vertexCount << std::endl;
		std::cout << " [debug]  Vertex normals  = " << vertNormCount << std::endl;
		std::cout << " [debug]  Texture coords  = " << vertTextureCount << std::endl;
		std::cout << " [debug] Defined Faces    = " << faceCount << std::endl;
		std::cout << " [debug]  Triangles       = " << triCount << std::endl;
		std::cout << " [debug]  Quads           = " << quadCount << std::endl;
	}

	// Reserve memory for vertices
	//reserve(vertexCount, triCount + 2 * quadCount);
	this->ReserveVertices(vertexCount);
	
	std::vector<Vector3> normals;
	normals.reserve(vertNormCount);

	std::vector<Vector2> textures;
	textures.reserve(vertTextureCount);

	// Reserve memory for polygons
	if (debugMode) {
		std::cout << " [debug] Generating " << childObjects.size() << " sub-objects" << std::endl;
	}
	for (auto obj = childObjects.cbegin(); obj != childObjects.cend(); obj++) {
		Material* mat = this->FindMaterial(obj->first);
		if (!mat || !mat->nAssociatedFaces) { // Undefined or unused material
			continue;
		}
		obj->second->Polygons()->Reserve(mat->nAssociatedFaces);
		if (debugMode) {
			std::cout << " [debug]  material=\"" << obj->first << "\", nTriangles=" << mat->nAssociatedFaces << std::endl;
		}
	}

	// Read the file again, this time generating the geometry
	f->clear();
	f->seekg(0);
	float values[4];
	uint32_t vertexIndicies[4];
	uint32_t textureIndicies[4];
	uint32_t normalIndicies[4];
	bool definedNormal;
	bool definedTexture;
	Vector3 vertexPosition;
	std::vector<std::string> indexStrings;
	lineCount = 0;
	while (ott::ReadLine(f, cmd, args)) { // Read model data
		lineCount++;
		if (cmd == ObjType::None)
			continue;
		switch (cmd) {
		case ObjType::None:
			break;
		case ObjType::MtlLib:
			break;
		case ObjType::UseMtl:
			currentMaterial = this->FindMaterial(args[1]);
			if (currentMaterial) {
				auto obj = childObjects.find(currentMaterial->sName);
				if (obj != childObjects.end()) { // Found sub-object
					currentChild = obj->second.get();
				}
			}
			break;
		case ObjType::Object:
			break;
		case ObjType::Group:
			break;
		case ObjType::Smooth:
			break;
		case ObjType::Vertex:
			for (int32_t i = 0; i < 3; i++) { // Ignore optional 'w' value
				vertexPosition[i] = std::stof(args[i + 1]);
			}
			this->ConvertToStandard(vertexPosition);
			this->AddVertex(vertexPosition);
			break;
		case ObjType::Normal:
			for (int32_t i = 0; i < 3; i++) { 
				values[i] = std::stof(args[i + 1]);
			}
			normals.push_back(Vector3(values[0], values[1], values[2]));
			break;
		case ObjType::Texture:
			for (int32_t i = 0; i < 2; i++) { // Ignore optional 'w' value
				values[i] = std::stof(args[i + 1]);
			}
			textures.push_back(Vector2(values[0], values[1]));
			break;
		case ObjType::Face:
			definedNormal = false;
			definedTexture = false;
			PolygonContainer* container = currentChild->Polygons();
			if (args.size() >= 5) { // Quad
				for (int32_t i = 0; i < 4; i++) {
					indexStrings.clear();
					ott::SplitString(args[i + 1], indexStrings, '/');
					if (indexStrings.empty() || indexStrings[0].empty()) {
						std::cout << " [error] On line " << lineCount << ", invalid argument given to quad constructor";
						break;
					}
					vertexIndicies[i] = std::stoul(indexStrings[0]) - 1; // Indexed from 1
					if (indexStrings.size() >= 2 && !indexStrings[1].empty()) { // Texture coords
						textureIndicies[i] = std::stoul(indexStrings[1]) - 1; // Indexed from 1
						if (textureIndicies[i] >= textures.size())
							std::cout << " [error] On line " << lineCount << ", invalid vertex texture index (" << textureIndicies[i] + 1 << ")" << std::endl;
						definedTexture = true;
					}
					if (indexStrings.size() >= 3 && !indexStrings[2].empty()) { // Vertex normal
						normalIndicies[i] = std::stoul(indexStrings[2]) - 1; // Indexed from 1
						if (normalIndicies[i] >= normals.size())
							std::cout << " [error] On line " << lineCount << ", invalid vertex normal index (" << normalIndicies[i] + 1 << ")" << std::endl;
						definedNormal = true;
					}
					if (vertexIndicies[i] >= vertices.size())
						std::cout << " [error] On line " << lineCount << ", invalid vertex index (" << vertexIndicies[i] + 1 << ")" << std::endl;
				}
				container->Add(vertices[vertexIndicies[3]], vertices[vertexIndicies[2]], vertices[vertexIndicies[1]], this); // currentChild??
				if (definedTexture) { // Update texture coordinates
					container->ModifyTextureMap(textures[textureIndicies[3]], textures[textureIndicies[2]], textures[textureIndicies[1]]);
				}
				if (definedNormal) { // Update vertex normal
					container->ModifyNormalVector(normals[normalIndicies[3]], normals[normalIndicies[2]], normals[normalIndicies[1]]);
				}
				container->Add(vertices[vertexIndicies[1]], vertices[vertexIndicies[0]], vertices[vertexIndicies[3]], this); // currentChild??
				if (definedTexture) { // Update texture coordinates
					container->ModifyTextureMap(textures[textureIndicies[1]], textures[textureIndicies[0]], textures[textureIndicies[3]]);
				}
				if (definedNormal) { // Update vertex normal
					container->ModifyNormalVector(normals[normalIndicies[1]], normals[normalIndicies[0]], normals[normalIndicies[3]]);
				}
			}
			else if (args.size() >= 4) { // Triangle
				for (int32_t i = 0; i < 3; i++) {
					indexStrings.clear();
					ott::SplitString(args[i + 1], indexStrings, '/');
					if (indexStrings.empty() || indexStrings[0].empty()) {
						std::cout << " [error] On line " << lineCount << ", invalid argument given to triangle constructor";
						break;
					}
					vertexIndicies[i] = std::stoul(indexStrings[0]) - 1; // Indexed from 1
					if (indexStrings.size() >= 2 && !indexStrings[1].empty()) { // Texture coords
						textureIndicies[i] = std::stoul(indexStrings[1]) - 1; // Indexed from 1
						if (textureIndicies[i] >= textures.size())
							std::cout << " [error] On line " << lineCount << ", invalid vertex texture index (" << textureIndicies[i] + 1 << ")" << std::endl;
						definedTexture = true;
					}
					if (indexStrings.size() >= 3 && !indexStrings[2].empty()) { // Vertex normal
						normalIndicies[i] = std::stoul(indexStrings[2]) - 1; // Indexed from 1
						if (normalIndicies[i] >= normals.size())
							std::cout << " [error] On line " << lineCount << ", invalid vertex normal index (" << normalIndicies[i] + 1 << ")" << std::endl;
						definedNormal = true;
					}
					if (vertexIndicies[i] >= vertices.size())
						std::cout << " [error] On line " << lineCount << ", invalid vertex index (" << vertexIndicies[i] + 1 << ")" << std::endl;
				}
				container->Add(vertices[vertexIndicies[2] - 1], vertices[vertexIndicies[1] - 1], vertices[vertexIndicies[0] - 1], this); // currentChild??
				if (definedTexture) { // Update texture coordinates
					container->ModifyTextureMap(textures[textureIndicies[2]], textures[textureIndicies[1]], textures[textureIndicies[0]]);
				}
				if (definedNormal) { // Update vertex normal
					container->ModifyNormalVector(normals[normalIndicies[2]], normals[normalIndicies[1]], normals[normalIndicies[0]]);
				}
			}
			break;
		}
	}

	if (debugMode)
		std::cout << " [debug] Finished reading Obj file" << std::endl;

	return 0;
}

bool ott::ModelObj::ReadMaterialFile(const std::string& filename) {
	std::ifstream matfile(filename.c_str());
	if (matfile.good() == false) {
		std::cout << " [error] Failed to open material file \"" << filename << "\"" << std::endl;
		return false;
	}
	std::string line;
	std::vector<std::string> args;
	Material* currentMaterial = nullptr;
	while (true) {
		std::getline(matfile, line);
		if (matfile.eof()) {
			break;
		}
		if (line.empty()) {
			continue;
		}
		ott::SplitString(line, args, ' ');
		if (args.empty() || args[0].front() == '#') {
			continue;
		}
		args[0] = ott::RemoveCharacter(args[0], '\t');
		if (args[0] == "newmtl") { // New material
			currentMaterial = new Material(args[1]);
			materials.push_back(std::unique_ptr<Material>(currentMaterial));
			if (debugMode)
				std::cout << " [debug] Defined new material \"" << currentMaterial->sName << "\"" << std::endl;
		}
		else if (args[0] == "Ns") { // Specular color component (0-1000)
			currentMaterial->specularRange = std::stof(args[1]);
		}
		else if (args[0] == "Ni") { // Index of refraction
			currentMaterial->indexOfRefraction = std::stof(args[1]);
		}
		else if (args[0] == "d") { // Opacity
			currentMaterial->opacity = std::stof(args[1]);
		}
		else if (args[0] == "Tr") { // Inverted opacity
			currentMaterial->opacity = 1.f - std::stof(args[1]);
		}
		else if (args[0] == "Tf") { // Transmission filter color (ignored)
		}
		else if (args[0] == "illum") { // Illumination model (ignored)
		}
		else if (args[0] == "Ka") { // Ambient color
			currentMaterial->ambient = ColorRGB(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
		}
		else if (args[0] == "Kd") { // Diffuse color
			currentMaterial->diffuse = ColorRGB(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
		}
		else if (args[0] == "Ks") { // Specular color
			currentMaterial->specular = ColorRGB(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
		}
		else if (args[0] == "Ke") { // Emissive color ???
		}
		else if (args[0] == "map_Ka") { // Ambient texture map
			std::string path = directory + "/";
			for (auto str = args.cbegin() + 1; str != args.cend(); str++) {
				path += *str;
			}
			currentMaterial->ambientTextureMap.reset(new Texture(path)); // Load texture image
		}
		else if (args[0] == "map_Kd") { // Diffuse texture map
			/*std::string path;
			for (auto str = args.cbegin() + 1; str != args.cend(); str++) {
				path += *str;
			}
			currentMaterial->diffuseTextureMap.reset(new Texture(path));*/
		}
		else if (args[0] == "map_Ks") { // Specular texture map
			/*std::string path;
			for (auto str = args.cbegin() + 1; str != args.cend(); str++) {
				path += *str;
			}
			currentMaterial->specularTextureMap.reset(new Texture(path));*/
		}
		else {
			std::cout << " [warning] Unknown material parameter (" << args[0] << ")" << std::endl;
		}
	}
	matfile.close();
	return true;
}

ott::ModelObj::Material* ott::ModelObj::FindMaterial(const std::string& name) {
	Material* retval = 0x0;
	for (auto mat = materials.cbegin(); mat != materials.cend(); mat++) {
		if (name == mat->get()->sName) {
			return mat->get();
		}
	}
	return retval;
}

/*
 * Private functions
 */

bool ott::ReadLine(std::ifstream* f, ObjType& type, std::vector<std::string>& args) {
	args.clear();
	std::string line;
	std::getline(*f, line);
	if (f->eof()) {
		return false;
	}
	if (line.empty()) {
		type = ObjType::None;
		return true;
	}
	if (line.back() == '\n' || line.back() == '\r') {
		line.erase(line.end() - 1);
	}
	if (line.empty()) {
		type = ObjType::None;
		return true;
	}
	ott::SplitString(line, args, ' ');
	if (args.empty() || args[0].front() == '#') {
		type = ObjType::None;
		return true;
	}
	if (args[0] == "mtllib") { // Load material 
		type = ObjType::MtlLib;
	}
	else if (args[0] == "usemtl") { // Use material
		type = ObjType::UseMtl;
	}
	else if (args[0] == "o") { // Object
		type = ObjType::Object;
	}
	else if (args[0] == "g") { // Group
		type = ObjType::Group;
	}
	else if (args[0] == "s") { // Smooth shading
		type = ObjType::Smooth;
	}
	else if (args[0] == "v") { // Vertex
		type = ObjType::Vertex;
	}
	else if (args[0] == "vn") { // Vertex normal
		type = ObjType::Normal;
	}
	else if (args[0] == "vt") { // Vertex texture coordinates
		type = ObjType::Texture;
	}
	else if (args[0] == "f") { // Polygons
		type = ObjType::Face;
	}
	return true;
}
