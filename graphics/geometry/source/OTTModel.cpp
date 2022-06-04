#include "OTTModel.hpp"

#include <iostream>
#include <sstream>

uint32_t ott::Model::Read(const std::string& fname) {
	path = fname;
	filename = fname;
	size_t index = fname.find_last_of('/');
	if (index != std::string::npos) {
		directory = fname.substr(0, index);
		filename = fname.substr(index + 1);
	}
	std::string inputExtension;
	index = filename.find_last_of('.');
	if (index != std::string::npos) {
		inputExtension = filename.substr(index + 1);
	}

	// Debug info
	if (debugMode) {
		std::cout << " [debug] directory=" << directory << std::endl;
		std::cout << " [debug] filename=" << filename << ", extension=" << inputExtension << std::endl;
	}
	if (inputExtension != extension)
		std::cout << " [warning] Input file extension (" << inputExtension << ") does not match expected extension (" << extension << ")" << std::endl;

	// Read the file
	std::ifstream ifile(fname, std::ios::binary);
	if (!ifile.good()) { // Failed to load file
		std::cout << " [error] Failed to load input file \"" << fname << "\"" << std::endl;
		return 0;
	}
	uint32_t retval = this->OnUserRead(&ifile);
	ifile.close();
	return retval;
}

std::string ott::Model::Hex8(const uint8_t& input) {
	std::stringstream stream;
	stream << std::hex << (int32_t)input;
	std::string output = stream.str();
	if (output.length() < 2) {
		output = "0" + output;
	}
	return output;
}

std::string ott::Model::Hex16(const uint16_t& input) {
	std::stringstream stream;
	stream << std::hex << (int32_t)input;
	std::string output = stream.str();
	if (output.length() < 4) {
		output = std::string(4 - output.length(), '0') + output;
	}
	return output;
}

uint8_t ott::Model::ReadUInt8(std::ifstream* f) {
	uint8_t val;
	f->read((char*)&val, sizeof(uint8_t));
	return val;
}

uint16_t ott::Model::ReadUInt16(std::ifstream* f) {
	uint16_t val;
	f->read((char*)&val, sizeof(uint16_t));
	return val;
}

uint32_t ott::Model::ReadUInt32(std::ifstream* f) {
	uint32_t val;
	f->read((char*)&val, sizeof(uint32_t));
	return val;
}

float ott::Model::ReadFloat(std::ifstream* f) {
	float val;
	f->read((char*)&val, sizeof(float));
	return val;
}

std::string ott::Model::ReadString(std::ifstream* f, const size_t& nBytes) {
	std::string str(nBytes, 0);
	f->read(&str[0], nBytes);
	std::string retval = "";
	for (auto c = str.rbegin(); c != str.rend(); c++) { // Convert the string to hex and reverse it (little-endian to big-endian)
		if (std::isspace(static_cast<uint8_t>(*c))) { // Strip invalid whitespace out of the number
			continue;
		}
		retval += ott::Model::Hex8(*c);
	}
	return retval;
}

void ott::Model::ConvertToStandard(Vector3& vec) {
	const float in = 0.0254f;
	const float ft = 0.3048f;
	const float m = 1.0f;
	const float dm = 1E-1f;
	const float cm = 1E-2f;
	const float mm = 1E-3f;
	const float um = 1E-6f;
	const float nm = 1E-9f;
	switch (conversion) {
	case UnitType::Inch:
		vec *= in;
		break;
	case UnitType::Foot:
		vec *= ft;
		break;
	case UnitType::Meter:
		vec *= m;
		break;
	case UnitType::Decimeter:
		vec *= dm;
		break;
	case UnitType::Centimeter:
		vec *= cm;
		break;
	case UnitType::Millimeter:
		vec *= mm;
		break;
	case UnitType::Micrometer:
		vec *= um;
		break;
	case UnitType::Nanometer:
		vec *= nm;
		break;
	default:
		break;
	}
}