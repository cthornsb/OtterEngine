#include "OTTModel3ds.hpp"

#include <graphics/core/OTTColor.hpp>

#include <iostream>

void ott::Model3ds::OnUserBuild() {
}

std::string ott::Model3ds::Chunk::ReadAsciiZ(std::ifstream* f) {
	char c;
	std::string retval;
	while (true) {
		nChunkBytes -= 1;
		f->read(&c, sizeof(char));
		if (c == '\0') {
			break;
		}
		retval += c;
	}
	return retval;
}

void ott::Model3ds::Chunk::SkipToNextChunk(std::ifstream* f) {
	f->seekg(nChunkBytes, std::ios_base::cur); // Skip payload
}

uint32_t ott::Model3ds::OnUserRead(std::ifstream* f) {
	Chunk primaryChunk(f);
	if (primaryChunk.nChunkID != 0x4d4d) {
		std::cout << " [error] Unrecognized primary chunk ID (" << Model::Hex16(primaryChunk.nChunkID) << ")" << std::endl;
		return false;
	}		
	while (this->ReadChunk(f)) { // Read model data
	}
	if (debugMode) {
		std::cout << " [debug] Finished reading 3ds file" << std::endl;
		std::cout << " [debug]  Model contains " << materials.size() << " materials" << std::endl;
	}
	return 0;
}

bool ott::Model3ds::ReadChunk(std::ifstream* f) {
	Chunk subChunk(f);
	if (f->eof())
		return false;
	ColorRGB color;
	uint16_t nFaces;
	uint16_t face;
	std::string str;
	if (debugMode)
		std::cout << " [debug]  Chunk - id=" << Model::Hex16(subChunk.nChunkID) << ", length=" << subChunk.nChunkBytes << " B" << std::endl;
	switch (subChunk.nChunkID) {
	case 0x0002: // Version number
		subChunk.nChunkBytes -= 4;
		nVersion = Model::ReadUInt32(f);
		if (debugMode)
			std::cout << " [debug] Version: " << nVersion << std::endl;
		break;
	case 0x3d3d: // Start of object mesh data
		break;
	case 0x3d3e: // Mesh version
		subChunk.nChunkBytes -= 4;
		nMeshVersion = Model::ReadUInt32(f);
		if (debugMode)
			std::cout << " [debug] Mesh version: " << nMeshVersion << std::endl;
		break;
	case 0xafff: // Start of material data
		subChunk.SkipToNextChunk(f); // Skip contents
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
		fScale = Model::ReadFloat(f);
		if (debugMode)
			std::cout << " [debug] Scale=" << fScale << std::endl;
		break;
	case 0x4000: // Object block
		sName = subChunk.ReadAsciiZ(f);
		if (debugMode)
			std::cout << " [debug] Name=" << sName << std::endl;
		break;
	case 0x4100: // Triangle mesh
		break;
	case 0x4110: // Vertex list
		nVertices = Model::ReadUInt16(f);
		this->ReserveVertices(nVertices);
		if (debugMode)
			std::cout << " [debug] nVertices=" << nVertices << std::endl;
		for (uint16_t i = 0; i < nVertices; i++) {
			vertexPosition[0] = Model::ReadFloat(f); // x
			vertexPosition[1] = Model::ReadFloat(f); // y
			vertexPosition[2] = Model::ReadFloat(f); // z
			this->ConvertToStandard(vertexPosition);
			this->AddVertex(vertexPosition);
		}
		break;
	case 0x4120: // Points list
		nPoints = Model::ReadUInt16(f);
		this->ReservePolygons(nPoints);
		if (debugMode)
			std::cout << " [debug] nPolygons=" << nPoints << std::endl;
		for (uint16_t i = 0; i < nPoints; i++) {
			polyIndicies[0] = Model::ReadUInt16(f); // p0
			polyIndicies[1] = Model::ReadUInt16(f); // p1
			polyIndicies[2] = Model::ReadUInt16(f); // p2
			polyIndicies[3] = Model::ReadUInt16(f); // ?
			if (polyIndicies[0] >= vertices.size() || polyIndicies[1] >= vertices.size() || polyIndicies[2] >= vertices.size()) {
				std::cout << " [error] 3ds polygon with invalid vertex index" << std::endl;
				continue;
			}
			this->AddTriangle(polyIndicies[0], polyIndicies[1], polyIndicies[2]);
		}
		break;
	case 0x4130: // Face material
		str = subChunk.ReadAsciiZ(f);
		nFaces = Model::ReadUInt16(f);
		if (debugMode) {
			std::cout << " [debug] Name=" << str << ", nFaces=" << nFaces << std::endl;
			std::cout << " [debug]  Material=" << this->FindMaterial(str) << std::endl;
		}
		for (uint16_t i = 0; i < nFaces; i++) {
			face = Model::ReadUInt16(f);
		}
		break;
	case 0x4140: // Vertex texture coords
		nVertices = Model::ReadUInt16(f);
		this->ReserveVertices(nVertices);
		if (debugMode)
			std::cout << " [debug] nTextureVertices=" << nVertices << std::endl;
		for (uint16_t i = 0; i < nVertices; i++) {
			textureCoords[0] = Model::ReadFloat(f); // u
			textureCoords[1] = Model::ReadFloat(f); // v
		}
		break;
	case 0x4150: // Face smoothing groups? (ignore)
		subChunk.SkipToNextChunk(f);
		break;
	case 0x4160: // Local unit vectors and object position
		for (uint16_t i = 0; i < 9; i++) { // uX, uY, and uZ
			fMatrix = Model::ReadFloat(f);
		}
		for (uint16_t i = 0; i < 3; i++) { // Center position
			fMatrix = Model::ReadFloat(f);
		}
		break;
	case 0xb000: // Start of keyframer data
		break;
	case 0xb002:
		subChunk.SkipToNextChunk(f);
		break;
	case 0xb008:
		subChunk.SkipToNextChunk(f);
		break;
	case 0xb009:
		subChunk.SkipToNextChunk(f);
		break;
	case 0xb00a:
		subChunk.SkipToNextChunk(f);
		break;
	default:
		if (debugMode)
			std::cout << " [debug] Unrecognized chunk ID (" << Model::Hex16(subChunk.nChunkID) << ")" << std::endl;
		subChunk.SkipToNextChunk(f);
		break;
	}	
	return true;
}

const ott::Model3ds::Material* ott::Model3ds::FindMaterial(const std::string& name) const {
	Material* retval = 0x0;
	for (auto mat = materials.cbegin(); mat != materials.cend(); mat++) {
		if (name == mat->sName) {
			return &(*mat);
		}
	}
	return retval;
}
