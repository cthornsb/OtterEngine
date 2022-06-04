#include "OTTModelStl.hpp"

#include <iostream>
#include <algorithm> // find
#include <cstring> // strcmp
#include <fstream>

ott::ModelStl::ModelStl() :
	Model("stl"),
	reversed(false)
{
}

void ott::ModelStl::OnUserBuild() {
}

uint32_t ott::ModelStl::OnUserRead(std::ifstream* f) {
	bool binary = this->IsBinaryFile(f);
	f->seekg(0, f->beg);
	uint32_t retval = 0;
	if (binary)
		retval = this->ReadBinary(f);
	else
		retval = this->ReadAscii(f);
	f->close();
	return retval;
}

uint32_t ott::ModelStl::ReadAscii(std::ifstream* f) {
	// Reserve enough space in the geometry vectors to avoid them resizing
	//reserve(nTriangles, nTriangles);
	std::vector<std::string> block;
	uint32_t linesRead = 0;
	std::string line;
	while (true) {
		std::getline(*f, line);
		if (f->eof() || (line.find("endsolid") != std::string::npos))
			break;

		linesRead++;

		// Read a new facet (triangle).
		if (line.find("endfacet") != std::string::npos) { // Finalize the facet.
			block.push_back(line);
			if (this->ReadAstBlock(block) == false) {
			}
			block.clear();
		}
		else {
			block.push_back(line);
		}
	}
	return (uint32_t)polys.Size();
}

uint32_t ott::ModelStl::ReadBinary(std::ifstream* f) {
	// Read the file header
	std::string header(80, 0);
	f->read(&header[0], 80);

	// Read 4 bytes from the file. This workaround is necessary because some
	// programs stupidly insert whitespace when writing numbers to the stl file.
	uint32_t nTriangles = Model::ReadUInt32(f);

	std::cout << " ModelStl: [debug] Stl header \"" << header << "\"" << std::endl;
	std::cout << " ModelStl: [debug] N=" << nTriangles << " triangles" << std::endl;

	// Reserve enough space in the geometry vectors to avoid them resizing
	this->Reserve(nTriangles, nTriangles);

	float vect[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint16_t attribute = 0;
	char byteRead = 0;
	bool invalidRead = false;
	for (uint32_t i = 0; i < nTriangles; i++) {
		for (uint16_t j = 0; j < 12; j++) { // Read the normal and vertex information
			vect[j] = Model::ReadFloat(f); // Floats assumed to be little-endian
		}
		attribute = Model::ReadUInt16(f); // Assumed to be little-endian
		if (f->eof()) {
			invalidRead = true;
			break;
		}
		for (uint16_t j = 0; j < attribute; j++) { // Read attribute bytes (typically not used)
			f->read(&byteRead, 1);
		}
		this->ReadStlBlock(vect);
	}

	// Set the center of the bounding box
	center[0] = this->SizeX() / 2 + minSize[0];
	center[1] = this->SizeY() / 2 + minSize[1];
	center[2] = this->SizeZ() / 2 + minSize[2];

	// Offset all vertices to the center the model
	std::vector<Vertex>::iterator iter;
	for (std::vector<Vertex>::iterator iter = vertices.Begin(); iter != vertices.End(); iter++) {
		iter->Offset(-center);
	}
	// Offset the center points of all polygons to the center the model
	for (std::vector<Polygon>::iterator iter = polys.Begin(); iter != polys.End(); iter++) {
		iter->OffsetCenter(-center);
	}

	if (!invalidRead) {
		std::cout << " ModelStl: [debug] Read " << nTriangles << " triangles from input .stl files" << std::endl;
		std::cout << " ModelStl: [debug]  Generated " << vertices.size() << " unique vertices and " << polys.Size() << " triangles" << std::endl;
		std::cout << " ModelStl: [debug]  Model has size (x=" << this->SizeX() << ", y=" << this->SizeY() << ", z=" << this->SizeZ() << ")" << std::endl;
		std::cout << " ModelStl: [debug]  x=(" << minSize[0] << ", " << maxSize[0] << ") y=(" << minSize[1] << ", " << maxSize[1] << ") z=(" << minSize[2] << ", " << maxSize[2] << ")" << std::endl;
	}
	else{
		std::cout << " ModelStl: [warning] Failed to read all " << nTriangles << " triangles specified in header!" << std::endl;
	}

	return (uint32_t)polys.Size();
}

void ott::ModelStl::ReadStlBlock(float* array) {
	Vertex* vertPtrs[3] = { 0, 0, 0 };
	Vector3 normal(array[0], array[1], array[2]);
	for (int32_t i = 1; i < 4; i++) {
		Vector3 vertex(array[3 * i], array[3 * i + 1], array[3 * i + 2]);
		this->ConvertToStandard(vertex);
		auto vec = std::find(vertices.Begin(), vertices.End(), vertex);
		if (vec != vertices.End()) {
			vertPtrs[i - 1] = &(*vec);
		}
		else { // found unique vertex
			vertPtrs[i - 1] = this->AddVertex(vertex);
		}
	}
	if (vertPtrs[0] && vertPtrs[1] && vertPtrs[2]) {
		if (normal != constants::kZeroVector) { // Stl file includes the normal vector
			if (!reversed) { // Normal vertices (right-hand rule)
				Polygon tri(vertPtrs[0], vertPtrs[1], vertPtrs[2], this);
				if (normal.Angle(tri.Normal()) > 0.01f) {
					std::cout << " ModelStl: [debug] Stl normal is anti-parallel to computed normal (angle=" << normal.Angle(tri.Normal()) << ")" << std::endl;
					std::cout << " ModelStl: [debug]  Switching to reverse vertex winding" << std::endl;
					//tri = Polygon(vertPtrs[0], vertPtrs[2], vertPtrs[1], this); // todo
					reversed = true;
				}
				polys.Add(tri);
			}
			else { // Reversed vertices
				Polygon tri(vertPtrs[0], vertPtrs[2], vertPtrs[1], this);
				polys.Add(tri);
			}
		}
		else {
			polys.Add(vertPtrs[0], vertPtrs[1], vertPtrs[2], this);
		}
	}
	else {
		std::cout << " error\n";
	}
}

bool ott::ModelStl::ReadAstBlock(const std::vector<std::string>& block) {
	int32_t vertexCount = 0;
	size_t index;
	for (auto iter = block.cbegin(); iter != block.cend(); iter++) {
		index = iter->find("normal");
		if (index != std::string::npos) { // Normal vector (not currently used)
			//Vector3 normal = getVectorFromString(iter->substr(index + 6));
		}
		index = iter->find("vertex");
		if (index != std::string::npos) { // Vertex vector
			if (vertexCount++ >= 3) {
				return false;
			}
			Vector3 vertex = this->GetVectorFromString(iter->substr(index + 6));
		}
	}
	return (isGood = (vertexCount == 3));
}

bool ott::ModelStl::IsBinaryFile(std::ifstream* f){
	char solidStr[6] = "";
	f->read(solidStr, 5);
	solidStr[5] = '\0';
	if (strcmp(solidStr, "solid") == 0) {
		return false; // Ascii stl file
	}
	return true;
}

ott::Vector3 ott::ModelStl::GetVectorFromString(const std::string& str) {
	// todo
	return constants::kZeroVector;
}
