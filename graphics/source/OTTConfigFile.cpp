#include <iostream>
#include <fstream>
#include <vector>

#include "OTTConfigFile.hpp"

bool ott::isNumeric(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if ((str[i] < '0' || str[i] > '9') && str[i] != '.')
			return false;
	return true;
}

unsigned int ott::splitString(const std::string& input, std::vector<std::string>& output, const unsigned char& delim/* = '\t'*/) {
	output.clear();
	size_t start = 0;
	size_t stop = 0;
	std::string temp;
	while (true) {
		stop = input.find(delim, start);
		if (stop == std::string::npos) {
			temp = input.substr(start);
			if (!temp.empty())
				output.push_back(temp);
			break;
		}
		temp = input.substr(start, stop - start);
		if (!temp.empty())
			output.push_back(temp);
		start = stop + 1;
	}
	return (unsigned int)output.size();
}

std::string ott::toUppercase(const std::string& str) {
	std::string retval = str;
	for (size_t i = 0; i < str.length(); i++) {
		if (retval[i] >= 'a' && str[i] <= 'z')
			retval[i] = (str[i] - 'a') + 'A';
	}
	return retval;
}

std::string ott::toLowercase(const std::string& str) {
	std::string retval = str;
	for (size_t i = 0; i < str.length(); i++) {
		if (retval[i] >= 'A' && str[i] <= 'Z')
			retval[i] = (str[i] - 'A') + 'a';
	}
	return retval;
}

ConfigFile::ConfigFile() : 
	bGood(false),
	filename(),
	currentName(),
	currentValue(),
	parameters()
{
}

ConfigFile::ConfigFile(const std::string& fname) : 
	bGood(false),
	filename(),
	currentName(),
	currentValue(),
	parameters()
{ 
	read(fname); 
}

bool ConfigFile::read(const std::string& fname){
	filename = fname;
	std::ifstream ifile(filename.c_str());
	if (!ifile.good())
		return false;
	std::string line;
	unsigned short lineCount = 0;
	while (true) {
		std::getline(ifile, line);
		lineCount++;
		if (ifile.eof() || !ifile.good())
			break;
		if (line.empty() || line[0] == '#') // Comments or blank lines
			continue;
		std::vector<std::string> args;
		ott::splitString(line, args, ' ');
		unsigned short argCount = 0;
		for(auto arg = args.begin(); arg != args.end(); arg++){
			if (arg->empty())
				continue;
			else if (argCount == 0)
				parameters[args.front()] = "";
			else if (argCount == 1)
				parameters[args.front()] = *arg;
			else {
				std::cout << " Warning! Extraneous arguments passed to parameter name \"" << args.front() << "\" on line " << lineCount << std::endl;
				break;
			}
			argCount++;
		}
	}
	ifile.close();
	return (bGood = true);
}

bool ConfigFile::search(const std::string& name, bool bRequiredArg/*=false*/) {
	auto element = parameters.find(name);
	if (element != parameters.end()) {
		currentName = element->first;
		currentValue = element->second;
		if (bRequiredArg && currentValue.empty()) {
			std::cout << " Warning! Missing required argument to parameter \"" << currentName << "\"\n";
			return false;
		}
		return true;
	}
	return false;
}

bool ConfigFile::searchBoolFlag(const std::string& name) {
	if (search(name, true))
		return getBoolFlag();
	return false;
}

std::string ConfigFile::getValue(const std::string& name) const {
	auto element = parameters.find(name);
	if (element != parameters.end())
		return element->second;
	return "";
}

bool ConfigFile::getBoolFlag(const std::string& name) const {
	std::string lower = ott::toLowercase(getValue(name));
	return (lower == "true" || (ott::isNumeric(lower) && std::stoul(lower) == 1));
}

unsigned char ConfigFile::getUChar(const std::string& name) const {
	return (unsigned char)std::stoul(getValue(name));
}

unsigned short ConfigFile::getUShort(const std::string& name) const {
	return (unsigned short)std::stoul(getValue(name));
}

unsigned int ConfigFile::getUInt(const std::string& name) const {
	return std::stoul(getValue(name));
}

float ConfigFile::getFloat(const std::string& name) const {
	return std::stof(getValue(name));
}

double ConfigFile::getDouble(const std::string& name) const {
	return std::stod(getValue(name));
}

std::string ConfigFile::getValue() const {
	return currentValue;
}

bool ConfigFile::getBoolFlag() const {
	std::string lower = ott::toLowercase(currentValue);
	return (lower == "true" || (ott::isNumeric(lower) && std::stoul(lower) == 1));
}

unsigned char ConfigFile::getUChar() const {
	return (unsigned char)std::stoul(currentValue);
}

unsigned short ConfigFile::getUShort() const {
	return (unsigned short)std::stoul(currentValue);
}

unsigned int ConfigFile::getUInt() const {
	return std::stoul(currentValue);
}

float ConfigFile::getFloat() const {
	return std::stof(currentValue);
}

double ConfigFile::getDouble() const {
	return std::stod(currentValue);
}

void ConfigFile::print() const {
	for (auto arg = parameters.begin(); arg != parameters.end(); arg++) {
		std::cout << arg->first << "\t\"" << arg->second << "\"\n";
	}
}
