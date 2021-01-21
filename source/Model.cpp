#include <iostream>
#include <sstream>

#include "Model.hpp"

unsigned int Model::read(const std::string& fname) {
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
	unsigned int retval = userRead(ifile);
	ifile.close();
	return retval;
}

std::string Model::getHex8(const unsigned char& input) {
	std::stringstream stream;
	stream << std::hex << (int)input;
	std::string output = stream.str();
	if (output.length() < 2)
		output = "0" + output;
	return output;
}

std::string Model::getHex16(const unsigned short& input) {
	std::stringstream stream;
	stream << std::hex << (int)input;
	std::string output = stream.str();
	if (output.length() < 4)
		output = std::string(4 - output.length(), '0') + output;
	return output;
}

void Model::readFromFile(std::ifstream& f, unsigned char& val) {
	f.read((char*)&val, 1);
}

void Model::readFromFile(std::ifstream& f, unsigned short& val) {
	f.read((char*)&val, 2);
}

void Model::readFromFile(std::ifstream& f, unsigned int& val) {
	f.read((char*)&val, 4);
}

void Model::readFromFile(std::ifstream& f, float& val) {
	f.read((char*)&val, 4);
}

void Model::convertToStandard(Vector3& vec) {
	const float in = 0.0254f;
	const float ft = 0.3048f;
	const float m = 1.0f;
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