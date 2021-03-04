#include <algorithm>
#include <iostream>
#include <fstream>

#include "ModelObj.hpp"
#include "ColorRGB.hpp"
#include "OTTTexture.hpp"

ModelObj::~ModelObj() {
}

void ModelObj::userBuild() {
	// Link material textures to sub-objects
	for (auto mat = materials.cbegin(); mat != materials.cend(); mat++) {
		Material* mptr = mat->get();
		if (!mptr->ambientTextureMap)
			continue;
		auto child = childObjects.find(mptr->sName);
		if (child == childObjects.end())
			continue;
		mptr->ambientTextureMap->getTexture(); // Convert to OpenGL texture
		if (debugMode)
			std::cout << " [debug] Converting to OpenGL texture (" << mptr->ambientTextureMap->getContext() << ")" << std::endl;
		child->second->setTexture(mptr->ambientTextureMap.get());
		mptr->ambientTextureMap->free(); // Free up image memory
	}

	// Finalize sub-objects and add them to the parent object
	for (auto obj = childObjects.cbegin(); obj != childObjects.cend(); obj++) {
		SubObject* child = obj->second.get();
		if (!child || child->getPolygonContainer()->empty()) // Skip empty objects
			continue;
		// This call will automatically generate the sub-object's VBO and free up conventional memory
		addChild(child);
	}
}

unsigned int ModelObj::userRead(std::ifstream& f) {
	// Add default material
	materials.push_back(std::unique_ptr<Material>(new Material()));
	Material* currentMaterial = materials.back().get();
	SubObject* currentChild = (childObjects[currentMaterial->sName] = std::unique_ptr<SubObject>(new SubObject(this))).get();

	std::string asciiString;
	std::vector<std::string> args;
	OBJ_TYPE cmd;

	// Read the file and count number of vertices and polygons
	unsigned int lineCount = 0;
	while (readLine(f, cmd, args)) { // Read model data
		lineCount++;
		switch (cmd) {
		case OBJ_TYPE::MTLLIB:
			asciiString = "";
			for (auto str = args.cbegin() + 1; str != args.cend(); str++) {
				asciiString += *str;
			}
			if (debugMode)
				std::cout << " [debug] Reading material file \"" << asciiString << "\"" << std::endl;
			readMaterialFile(directory + "/" + asciiString);
			break;
		case OBJ_TYPE::USEMTL:
			currentMaterial = findMaterial(args[1]);
			if (currentMaterial) {
				auto obj = childObjects.find(currentMaterial->sName);
				if (obj == childObjects.end()) { // New child object
					currentChild = (childObjects[currentMaterial->sName] = std::unique_ptr<SubObject>(new SubObject(this))).get();
					if (debugMode)
						std::cout << " [debug] Adding new sub-object associated with material \"" << currentMaterial->sName << "\"" << std::endl;
				}
				else {
					currentChild = obj->second.get();
				}
			}
			else
				std::cout << " [error] On line " << lineCount << ", undefined material \"" << args[1] << "\"" << std::endl;
			break;
		case OBJ_TYPE::OBJECT:
			objectName = args[1];
			if (debugMode)
				std::cout << " [debug] Object Name: " << objectName << std::endl;
			break;
		case OBJ_TYPE::GROUP:
			groupName = args[1];
			if (debugMode)
				std::cout << " [debug] Group Name: " << groupName << std::endl;
			break;
		case OBJ_TYPE::SMOOTH:
			break;
		case OBJ_TYPE::VERTEX:
			vertexCount++;
			break;
		case OBJ_TYPE::NORMAL:
			vertNormCount++;
			break;
		case OBJ_TYPE::TEXTURE:
			vertTextureCount++;
			break;
		case OBJ_TYPE::FACE:
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
	reserveVertices(vertexCount);
	
	std::vector<Vector3> normals;
	normals.reserve(vertNormCount);

	std::vector<Vector2> textures;
	textures.reserve(vertTextureCount);

	// Reserve memory for polygons
	if(debugMode)
		std::cout << " [debug] Generating " << childObjects.size() << " sub-objects" << std::endl;
	for (auto obj = childObjects.cbegin(); obj != childObjects.cend(); obj++) {
		Material* mat = findMaterial(obj->first);
		if (!mat || !mat->nAssociatedFaces) // Undefined or unused material
			continue;
		obj->second->getPolygonContainer()->reserve(mat->nAssociatedFaces);
		if (debugMode)
			std::cout << " [debug]  material=\"" << obj->first << "\", nTriangles=" << mat->nAssociatedFaces << std::endl;
	}

	// Read the file again, this time generating the geometry
	f.clear();
	f.seekg(0);
	float values[4];
	unsigned int vertexIndicies[4];
	unsigned int textureIndicies[4];
	unsigned int normalIndicies[4];
	bool definedNormal;
	bool definedTexture;
	Vector3 vertexPosition;
	std::vector<std::string> indexStrings;
	lineCount = 0;
	while (readLine(f, cmd, args)) { // Read model data
		lineCount++;
		if (cmd == OBJ_TYPE::NONE)
			continue;
		switch (cmd) {
		case OBJ_TYPE::NONE:
			break;
		case OBJ_TYPE::MTLLIB:
			break;
		case OBJ_TYPE::USEMTL:
			currentMaterial = findMaterial(args[1]);
			if (currentMaterial) {
				auto obj = childObjects.find(currentMaterial->sName);
				if (obj != childObjects.end()) // Found sub-object
					currentChild = obj->second.get();
			}
			break;
		case OBJ_TYPE::OBJECT:
			break;
		case OBJ_TYPE::GROUP:
			break;
		case OBJ_TYPE::SMOOTH:
			break;
		case OBJ_TYPE::VERTEX:
			for (int i = 0; i < 3; i++) { // Ignore optional 'w' value
				vertexPosition[i] = std::stof(args[i + 1]);
			}
			convertToStandard(vertexPosition);
			addVertex(vertexPosition);
			break;
		case OBJ_TYPE::NORMAL:
			for (int i = 0; i < 3; i++) { 
				values[i] = std::stof(args[i + 1]);
			}
			normals.push_back(Vector3(values[0], values[1], values[2]));
			break;
		case OBJ_TYPE::TEXTURE:
			for (int i = 0; i < 2; i++) { // Ignore optional 'w' value
				values[i] = std::stof(args[i + 1]);
			}
			textures.push_back(Vector2(values[0], values[1]));
			break;
		case OBJ_TYPE::FACE:
			definedNormal = false;
			definedTexture = false;
			PolygonContainer* container = currentChild->getPolygonContainer();
			if (args.size() >= 5) { // Quad
				for (int i = 0; i < 4; i++) {
					indexStrings.clear();
					splitString(args[i + 1], indexStrings, '/');
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
				container->add(vertices[vertexIndicies[3]], vertices[vertexIndicies[2]], vertices[vertexIndicies[1]], this); // currentChild??
				if(definedTexture) // Update texture coordinates
					container->modifyTextureMap(textures[textureIndicies[3]], textures[textureIndicies[2]], textures[textureIndicies[1]]);
				if(definedNormal) // Update vertex normal
					container->modifyNormalVector(normals[normalIndicies[3]], normals[normalIndicies[2]], normals[normalIndicies[1]]);
				container->add(vertices[vertexIndicies[1]], vertices[vertexIndicies[0]], vertices[vertexIndicies[3]], this); // currentChild??
				if (definedTexture) // Update texture coordinates
					container->modifyTextureMap(textures[textureIndicies[1]], textures[textureIndicies[0]], textures[textureIndicies[3]]);
				if (definedNormal) // Update vertex normal
					container->modifyNormalVector(normals[normalIndicies[1]], normals[normalIndicies[0]], normals[normalIndicies[3]]);
			}
			else if (args.size() >= 4) { // Triangle
				for (int i = 0; i < 3; i++) {
					indexStrings.clear();
					splitString(args[i + 1], indexStrings, '/');
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
				container->add(vertices[vertexIndicies[2] - 1], vertices[vertexIndicies[1] - 1], vertices[vertexIndicies[0] - 1], this); // currentChild??
				if (definedTexture) // Update texture coordinates
					container->modifyTextureMap(textures[textureIndicies[2]], textures[textureIndicies[1]], textures[textureIndicies[0]]);
				if (definedNormal) // Update vertex normal
					container->modifyNormalVector(normals[normalIndicies[2]], normals[normalIndicies[1]], normals[normalIndicies[0]]);
			}
			break;
		}
	}

	if (debugMode)
		std::cout << " [debug] Finished reading Obj file" << std::endl;

	return 0;
}

bool ModelObj::readLine(std::ifstream& f, OBJ_TYPE& type, std::vector<std::string>& args) {
	args.clear();
	std::string line;
	std::getline(f, line);
	if (f.eof())
		return false;
	if (line.empty()) {
		type = OBJ_TYPE::NONE;
		return true;
	}
	if (line.back() == '\n' || line.back() == '\r')
		line.erase(line.end() - 1);
	if (line.empty()) {
		type = OBJ_TYPE::NONE;
		return true;
	}
	splitString(line, args, ' ');
	if (args.empty() || args[0].front() == '#') {
		type = OBJ_TYPE::NONE;
		return true;
	}
	if (args[0] == "mtllib") { // Load material 
		type = OBJ_TYPE::MTLLIB;
	}
	else if (args[0] == "usemtl") { // Use material
		type = OBJ_TYPE::USEMTL;
	}
	else if (args[0] == "o") { // Object
		type = OBJ_TYPE::OBJECT;
	}
	else if (args[0] == "g") { // Group
		type = OBJ_TYPE::GROUP;
	}
	else if (args[0] == "s") { // Smooth shading
		type = OBJ_TYPE::SMOOTH;
	}
	else if (args[0] == "v") { // Vertex
		type = OBJ_TYPE::VERTEX;
	}
	else if (args[0] == "vn") { // Vertex normal
		type = OBJ_TYPE::NORMAL;
	}
	else if (args[0] == "vt") { // Vertex texture coordinates
		type = OBJ_TYPE::TEXTURE;
	}
	else if (args[0] == "f") { // Polygons
		type = OBJ_TYPE::FACE;
	}
	return true;
}

bool ModelObj::readMaterialFile(const std::string& filename) {
	std::ifstream matfile(filename.c_str());
	if (!matfile.good()) {
		std::cout << " [error] Failed to open material file \"" << filename << "\"" << std::endl;
		return false;
	}
	std::string line;
	std::vector<std::string> args;
	Material* currentMaterial = 0x0;
	while (true) {
		std::getline(matfile, line);
		if (matfile.eof())
			break;
		if (line.empty())
			continue;
		splitString(line, args, ' ');
		if (args.empty() || args[0].front() == '#')
			continue;
		args[0] = removeCharacter(args[0], '\t');
		if (args[0] == "newmtl") { // New material
			materials.push_back(std::unique_ptr<Material>(new Material(args[1])));
			currentMaterial = materials.back().get();
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
			currentMaterial->ambientTextureMap.reset(new OTTTexture(path)); // Load texture image
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

std::string ModelObj::removeCharacter(const std::string& input, const char& ch) {
	std::string retval;
	for (size_t i = 0; i < input.length(); i++) {
		if (input[i] == ch)
			continue;
		retval += input[i];
	}
	return retval;
}

unsigned int ModelObj::splitString(const std::string& input, std::vector<std::string>& output, const char& delim/* = ' '*/) {
	output.clear();
	size_t start = 0;
	size_t stop = 0;
	while (true) {
		stop = input.find_first_of(delim, start);
		if (stop == std::string::npos) {
			std::string str = input.substr(start);
			if (!str.empty())
				output.push_back(str);
			break;
		}
		output.push_back(input.substr(start, stop - start));
		start = input.find_first_not_of(delim, stop + 1);
		if (start == std::string::npos)
			break;
	}
	return output.size();
}

ModelObj::Material* ModelObj::findMaterial(const std::string& name) {
	Material* retval = 0x0;
	for (auto mat = materials.cbegin(); mat != materials.cend(); mat++) {
		if (name == mat->get()->sName) {
			return mat->get();
		}
	}
	return retval;
}
