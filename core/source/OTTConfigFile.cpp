#include "OTTConfigFile.hpp"

#include <core/OTTStringOps.hpp>

#include <iostream>
#include <fstream>
#include <vector>

ott::ConfigFile::ConfigFile() : 
	bGood(false),
	filename(),
	currentName(),
	currentValue(),
	parameters()
{
}

ott::ConfigFile::ConfigFile(const std::string& fname) : 
	bGood(false),
	filename(),
	currentName(),
	currentValue(),
	parameters()
{ 
	read(fname); 
}

bool ott::ConfigFile::read(const std::string& fname){
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
		ott::SplitString(line, args, ' ');
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

bool ott::ConfigFile::search(const std::string& name, bool bRequiredArg/*=false*/) {
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

bool ott::ConfigFile::searchBoolFlag(const std::string& name) {
	if (search(name, true))
		return GetBoolean();
	return false;
}

std::string ott::ConfigFile::Value(const std::string& name) const {
	auto element = parameters.find(name);
	if (element != parameters.end())
		return element->second;
	return "";
}

bool ott::ConfigFile::GetBoolean(const std::string& name) const {
	std::string lower = ott::StringToLowercase(this->Value(name));
	return (lower == "true" || (ott::StringIsNumeric(lower) && std::stoul(lower) == 1));
}

unsigned char ott::ConfigFile::GetUChar(const std::string& name) const {
	return (unsigned char)std::stoul(this->Value(name));
}

unsigned short ott::ConfigFile::GetUShort(const std::string& name) const {
	return (unsigned short)std::stoul(this->Value(name));
}

unsigned int ott::ConfigFile::GetUInt(const std::string& name) const {
	return std::stoul(this->Value(name));
}

float ott::ConfigFile::GetFloat(const std::string& name) const {
	return std::stof(this->Value(name));
}

double ott::ConfigFile::GetDouble(const std::string& name) const {
	return std::stod(this->Value(name));
}

std::string ott::ConfigFile::Value() const {
	return currentValue;
}

bool ott::ConfigFile::GetBoolean() const {
	std::string lower = ott::StringToLowercase(currentValue);
	return (lower == "true" || (ott::StringIsNumeric(lower) && std::stoul(lower) == 1));
}

unsigned char ott::ConfigFile::GetUChar() const {
	return (unsigned char)std::stoul(currentValue);
}

unsigned short ott::ConfigFile::GetUShort() const {
	return (unsigned short)std::stoul(currentValue);
}

unsigned int ott::ConfigFile::GetUInt() const {
	return std::stoul(currentValue);
}

float ott::ConfigFile::GetFloat() const {
	return std::stof(currentValue);
}

double ott::ConfigFile::GetDouble() const {
	return std::stod(currentValue);
}

void ott::ConfigFile::Print() const {
	for (auto arg = parameters.begin(); arg != parameters.end(); arg++) {
		std::cout << arg->first << "\t\"" << arg->second << "\"\n";
	}
}
