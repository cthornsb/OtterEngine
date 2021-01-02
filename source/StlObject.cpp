#include <sstream>
#include <iostream>
#include <fstream>

#include "StlObject.hpp"

StlObject::StlObject() :
	object(),
	isGood(false),
	reversed(false),
	conversion(UNITS::INCH)
{
}

void StlObject::convertToStandard(vector3& vec) {
	const float in = 0.0254f;
	const float ft = 0.3048f;
	const float m  = 1.0f;
	const float dm = 1E-1f;
	const float cm = 1E-2f;
	const float mm = 1E-3f;
	const float um = 1E-6f;
	const float nm = 1E-9f;
	switch (conversion) {
	case UNITS::INCH:
		vec *= in;
		break;
	case UNITS::FOOT:
		vec *= ft;
		break;
	case UNITS::METER:
		vec *= m;
		break;
	case UNITS::DECIMETER:
		vec *= dm;
		break;
	case UNITS::CENTIMETER:
		vec *= cm;
		break;
	case UNITS::MILLIMETER:
		vec *= mm;
		break;
	case UNITS::MICROMETER:
		vec *= um;
		break;
	case UNITS::NANOMETER:
		vec *= nm;
		break;
	default:
		break;
	}
}

void StlObject::userBuild() {
}

unsigned int StlObject::read(const std::string& fname, const UNITS& unit/*=UNITS::INCH*/) {
	std::ifstream file(fname.c_str(), std::ios::binary);
	if (!file.good())
		return 0;
	conversion = unit;
	bool binary = isBinaryFile(&file);
	file.seekg(0, file.beg);
	unsigned int retval = 0;
	if (binary)
		retval = readBinary(&file);
	else
		retval = readAscii(&file);
	file.close();
	return retval;
}

unsigned int StlObject::readAscii(std::ifstream* f) {
	// Reserve enough space in the geometry vectors to avoid them resizing
	//reserve(nTriangles, nTriangles);
	std::vector<std::string> block;
	unsigned int linesRead = 0;
	std::string line;
	while (true) {
		std::getline(*f, line);
		if (f->eof() || (line.find("endsolid") != std::string::npos))
			break;

		linesRead++;

		// Read a new facet (triangle).
		if (line.find("endfacet") != std::string::npos) { // Finalize the facet.
			block.push_back(line);
			if (!readAstBlock(block)) {
			}
			block.clear();
		}
		else {
			block.push_back(line);
		}
	}
	return (unsigned int)polys.size();
}

unsigned int StlObject::readBinary(std::ifstream* f) {
	// Read the file header
	std::string header(80, 0);
	f->read(&header[0], 80);

	// Read 4 bytes from the file. This workaround is necessary because some
	// programs stupidly insert whitespace when writing numbers to the stl file.
	unsigned int nTriangles = readUIntFromFile(f);

	std::cout << " StlObject: [debug] Stl header \"" << header << "\"" << std::endl;
	std::cout << " StlObject: [debug] N=" << nTriangles << " triangles" << std::endl;

	// Reserve enough space in the geometry vectors to avoid them resizing
	reserve(nTriangles, nTriangles);

	float vect[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned short attribute = 0;
	char byteRead = 0;
	bool invalidRead = false;
	for (unsigned int i = 0; i < nTriangles; i++) {
		for (unsigned short j = 0; j < 12; j++) { // Read the normal and vertex information
			vect[j] = readFloatFromFile(f); // Floats assumed to be little-endian
		}
		attribute = readUShortFromFile(f); // Assumed to be little-endian
		if (f->eof()) {
			invalidRead = true;
			break;
		}
		for (unsigned short j = 0; j < attribute; j++) { // Read attribute bytes (typically not used)
			f->read(&byteRead, 1);
		}
		readStlBlock(vect);
	}

	// Set the center of the bounding box
	center.x = getSizeX() / 2 + minSize[0];
	center.y = getSizeY() / 2 + minSize[1];
	center.z = getSizeZ() / 2 + minSize[2];

	// Offset all vertices to the center the model
	std::vector<Vertex>::iterator iter;
	for (std::vector<Vertex>::iterator iter = vertices.begin(); iter != vertices.end(); iter++) {
		iter->offsetPosition(-center);
	}
	// Offset the center points of all polygons to the center the model
	for (std::vector<triangle>::iterator iter = polys.begin(); iter != polys.end(); iter++) {
		iter->offsetPosition(-center);
	}

	if (!invalidRead) {
		std::cout << " StlObject: [debug] Read " << nTriangles << " triangles from input .stl files" << std::endl;
		std::cout << " StlObject: [debug]  Generated " << vertices.size() << " unique vertices and " << polys.size() << " triangles" << std::endl;
		std::cout << " StlObject: [debug]  Model has size (x=" << getSizeX() << ", y=" << getSizeY() << ", z=" << getSizeZ() << ")" << std::endl;
		std::cout << " StlObject: [debug]  x=(" << minSize[0] << ", " << maxSize[0] << ") y=(" << minSize[1] << ", " << maxSize[1] << ") z=(" << minSize[2] << ", " << maxSize[2] << ")" << std::endl;
	}
	else{
		std::cout << " StlObject: [warning] Failed to read all " << nTriangles << " triangles specified in header!" << std::endl;
	}

	return (unsigned int)polys.size();
}

void StlObject::readStlBlock(float* array) {
	Vertex* vertPtrs[3] = { 0, 0, 0 };
	vector3 normal(array[0], array[1], array[2]);
	for (int i = 1; i < 4; i++) {
		vector3 vertex(array[3 * i], array[3 * i + 1], array[3 * i + 2]);
		convertToStandard(vertex);
		auto vec = std::find(vertices.begin(), vertices.end(), vertex);
		if (vec != vertices.end())
			vertPtrs[i-1] = &(*vec);
		else { // found unique vertex
			vertPtrs[i - 1] = addVertex(vertex);
		}
	}
	if (vertPtrs[0] && vertPtrs[1] && vertPtrs[2]) {
		if (normal != zeroVector) { // Stl file includes the normal vector
			if (!reversed) { // Normal vertices (right-hand rule)
				triangle tri(vertPtrs[0], vertPtrs[1], vertPtrs[2], this);
				if (normal.angle(tri.getNormal()) > 0.01f) {
					std::cout << " StlObject: [debug] Stl normal is anti-parallel to computed normal (angle=" << normal.angle(tri.getNormal()) << ")" << std::endl;
					std::cout << " StlObject: [debug]  Switching to reverse vertex winding" << std::endl;
					tri = triangle(vertPtrs[0], vertPtrs[2], vertPtrs[1], this);
					reversed = true;
				}
				polys.add(tri, &vertices);
			}
			else { // Reversed vertices
				triangle tri(vertPtrs[0], vertPtrs[2], vertPtrs[1], this);
				polys.add(tri, &vertices);
			}
		}
		else
			polys.add(vertPtrs[0], vertPtrs[1], vertPtrs[2], &vertices, this);
	}
	else {
		std::cout << " error\n";
	}
}

bool StlObject::readAstBlock(const std::vector<std::string>& block) {
	int vertexCount = 0;
	size_t index;
	for (auto iter = block.cbegin(); iter != block.cend(); iter++) {
		index = iter->find("normal");
		if (index != std::string::npos) { // Normal vector (not currently used)
			//vector3 normal = getVectorFromString(iter->substr(index + 6));
		}
		index = iter->find("vertex");
		if (index != std::string::npos) { // Vertex vector
			if (vertexCount++ >= 3)
				return false;
			vector3 vertex = getVectorFromString(iter->substr(index + 6));
		}
	}
	return (isGood = (vertexCount == 3));
}

bool StlObject::isBinaryFile(std::ifstream* f){
	char solidStr[6] = "";
	f->read(solidStr, 5);
	solidStr[5] = '\0';
	if (strcmp(solidStr, "solid") == 0) {
		return false; // Ascii stl file
	}
	return true;
}

vector3 StlObject::getVectorFromString(const std::string& str) {
	return zeroVector;
}

std::string StlObject::getHex(const unsigned char& input) {
	std::stringstream stream;
	stream << std::hex << (int)input;
	std::string output = stream.str();
	if (output.length() < 2)
		output = "0" + output;
	return output;
}

std::string StlObject::readString(std::ifstream* f, const size_t& nBytes) {
	std::string str(nBytes, 0);
	f->read(&str[0], nBytes);
	std::string retval = "";
	for (auto c = str.rbegin(); c != str.rend(); c++) { // Convert the string to hex and reverse it (little-endian to big-endian)
		if (std::isspace(static_cast<unsigned char>(*c))) // Strip invalid whitespace out of the number
			continue;
		retval += getHex(*c);
	}
	return retval;
}

float StlObject::readFloatFromFile(std::ifstream* f) {
	float dummy = 0;
	f->read(reinterpret_cast<char*>(&dummy), 4);
	return dummy;
}

unsigned int StlObject::readUIntFromFile(std::ifstream* f) {
	std::string revStr = readString(f, 4);
	if (revStr.empty())
		return 0;
	return std::stoul(revStr, 0, 16);
}

unsigned short StlObject::readUShortFromFile(std::ifstream* f) {
	std::string revStr = readString(f, 2);
	if (revStr.empty())
		return 0;
	return (unsigned short)std::stoul(revStr, 0, 16);
}