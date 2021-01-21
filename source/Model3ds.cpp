#include <iostream>

#include "Model3ds.hpp"
#include "ColorRGB.hpp"

void Model3ds::userBuild() {
}

std::string Model3ds::Chunk::getAsciiZ(std::ifstream& f) {
	char c;
	std::string retval;
	while (true) {
		nChunkBytes -= 1;
		f.read(&c, 1);
		if (c == '\0')
			break;
		retval += c;
	}
	return retval;
}

void Model3ds::Chunk::skipToNextChunk(std::ifstream& f) {
	f.seekg(nChunkBytes, std::ios_base::cur); // Skip payload
}

unsigned int Model3ds::userRead(std::ifstream& f) {
	Chunk primaryChunk(f);
	if (primaryChunk.nChunkID != 0x4d4d) {
		std::cout << " [error] Unrecognized primary chunk ID (" << getHex16(primaryChunk.nChunkID) << ")" << std::endl;
		return false;
	}		
	while (readChunk(f)) { // Read model data
	}
	if (debugMode) {
		std::cout << " [debug] Finished reading 3ds file" << std::endl;
		std::cout << " [debug]  Model contains " << materials.size() << " materials" << std::endl;
	}
	return 0;
}

bool Model3ds::readChunk(std::ifstream& f) {
	Chunk subChunk(f);
	if (f.eof())
		return false;
	ColorRGB color;
	unsigned short nFaces;
	unsigned short face;
	std::string str;
	if (debugMode)
		std::cout << " [debug]  Chunk - id=" << getHex16(subChunk.nChunkID) << ", length=" << subChunk.nChunkBytes << " B" << std::endl;
	switch (subChunk.nChunkID) {
	case 0x0002: // Version number
		subChunk.nChunkBytes -= 4;
		readFromFile(f, nVersion);
		if (debugMode)
			std::cout << " [debug] Version: " << nVersion << std::endl;
		break;
	case 0x3d3d: // Start of object mesh data
		break;
	case 0x3d3e: // Mesh version
		subChunk.nChunkBytes -= 4;
		readFromFile(f, nMeshVersion);
		if (debugMode)
			std::cout << " [debug] Mesh version: " << nMeshVersion << std::endl;
		break;
	case 0xafff: // Start of material data
		subChunk.skipToNextChunk(f); // Skip contents
		break;
	/*case 0xa000: // Material name
		str = subChunk.getAsciiZ(f);
		std::cout << " [debug] Material name=" << str << std::endl;
		materials.push_back(Material(str));
		break;
	case 0xa010: // Ambient color
		readFromFile(f, color.r); // r
		readFromFile(f, color.g); // g
		readFromFile(f, color.b); // b
		std::cout << " [debug] ambient=(" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
		break;
	case 0xa020: // Diffuse color
		readFromFile(f, color.r); // r
		readFromFile(f, color.g); // g
		readFromFile(f, color.b); // b
		std::cout << " [debug] diffuse=(" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
		break;
	case 0xa030: // Specular color
		readFromFile(f, color.r); // r
		readFromFile(f, color.g); // g
		readFromFile(f, color.b); // b
		std::cout << " [debug] specular=(" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
		break;
	case 0xa040:
		break;
	case 0xa200: // Texture map 1
		str = subChunk.getAsciiZ(f);
		std::cout << "HERE 200! size=" << subChunk.nChunkBytes << ", str=" << str << std::endl;
		subChunk.skipToNextChunk(f); // Skip remaining contents
		break;
	case 0xa220: // Reflection map
		break;
	case 0xa230: // Bump map
		break;*/
	case 0x0100: // Master scale
		subChunk.nChunkBytes -= 4;
		f.read((char*)&fScale, 4);
		if (debugMode)
			std::cout << " [debug] Scale=" << fScale << std::endl;
		break;
	case 0x4000: // Object block
		sName = subChunk.getAsciiZ(f);
		if (debugMode)
			std::cout << " [debug] Name=" << sName << std::endl;
		break;
	case 0x4100: // Triangle mesh
		break;
	case 0x4110: // Vertex list
		readFromFile(f, nVertices);
		reserveVertices(nVertices);
		if (debugMode)
			std::cout << " [debug] nVertices=" << nVertices << std::endl;
		for (unsigned short i = 0; i < nVertices; i++) {
			readFromFile(f, vertexPosition[0]); // x
			readFromFile(f, vertexPosition[1]); // y
			readFromFile(f, vertexPosition[2]); // z
			convertToStandard(vertexPosition);
			addVertex(vertexPosition);
		}
		break;
	case 0x4120: // Points list
		readFromFile(f, nPoints);
		reservePolygons(nPoints);
		if (debugMode)
			std::cout << " [debug] nPolygons=" << nPoints << std::endl;
		for (unsigned short i = 0; i < nPoints; i++) {
			readFromFile(f, polyIndicies[0]); // p0
			readFromFile(f, polyIndicies[1]); // p1
			readFromFile(f, polyIndicies[2]); // p2
			readFromFile(f, polyIndicies[3]); // ?
			if (polyIndicies[0] >= vertices.size() || polyIndicies[1] >= vertices.size() || polyIndicies[2] >= vertices.size()) {
				std::cout << " [error] 3ds polygon with invalid vertex index" << std::endl;
				continue;
			}
			addTriangle(polyIndicies[0], polyIndicies[1], polyIndicies[2]);
		}
		break;
	case 0x4130: // Face material
		str = subChunk.getAsciiZ(f);
		readFromFile(f, nFaces);
		if (debugMode) {
			std::cout << " [debug] Name=" << str << ", nFaces=" << nFaces << std::endl;
			std::cout << " [debug]  Material=" << findMaterial(str) << std::endl;
		}
		for (unsigned short i = 0; i < nFaces; i++) {
			readFromFile(f, face);
		}
		break;
	case 0x4140: // Vertex texture coords
		readFromFile(f, nVertices);
		reserveVertices(nVertices);
		if (debugMode)
			std::cout << " [debug] nTextureVertices=" << nVertices << std::endl;
		for (unsigned short i = 0; i < nVertices; i++) {
			readFromFile(f, textureCoords[0]);
			readFromFile(f, textureCoords[1]);
		}
		break;
	case 0x4150: // Face smoothing groups? (ignore)
		subChunk.skipToNextChunk(f);
		break;
	case 0x4160: // Local unit vectors and object position
		for (unsigned short i = 0; i < 9; i++) { // uX, uY, and uZ
			readFromFile(f, fMatrix);
		}
		for (unsigned short i = 0; i < 3; i++) { // Center position
			readFromFile(f, fMatrix);
		}
		break;
	case 0xb000: // Start of keyframer data
		break;
	case 0xb002:
		subChunk.skipToNextChunk(f);
		break;
	case 0xb008:
		subChunk.skipToNextChunk(f);
		break;
	case 0xb009:
		subChunk.skipToNextChunk(f);
		break;
	case 0xb00a:
		subChunk.skipToNextChunk(f);
		break;
	default:
		if (debugMode)
			std::cout << " [debug] Unrecognized chunk ID (" << getHex16(subChunk.nChunkID) << ")" << std::endl;
		subChunk.skipToNextChunk(f);
		break;
	}	
	return true;
}

const Model3ds::Material* Model3ds::findMaterial(const std::string& name) const {
	Material* retval = 0x0;
	for (auto mat = materials.cbegin(); mat != materials.cend(); mat++) {
		if (name == mat->sName) {
			return &(*mat);
		}
	}
	return retval;
}