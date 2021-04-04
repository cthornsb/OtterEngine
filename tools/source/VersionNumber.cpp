#include <sstream>

#include "VersionNumber.hpp"

using namespace ott;

bool VersionNumber::operator >= (const VersionNumber& rhs) const {
	return (
		nVersionMajor >= rhs.nVersionMajor &&
		nVersionMinor >= rhs.nVersionMinor &&
		nVersionRevision >= rhs.nVersionRevision
		);
}

bool VersionNumber::operator <= (const VersionNumber& rhs) const {
	return (
		nVersionMajor <= rhs.nVersionMajor &&
		nVersionMinor <= rhs.nVersionMinor &&
		nVersionRevision <= rhs.nVersionRevision
		);
}

bool VersionNumber::operator == (const VersionNumber& rhs) const {
	return (
		nVersionMajor == rhs.nVersionMajor &&
		nVersionMinor == rhs.nVersionMinor &&
		nVersionRevision == rhs.nVersionRevision
		);
}

std::string VersionNumber::getString() const {
	std::stringstream stream;
	stream << nVersionMajor << "." << nVersionMinor << "." << nVersionRevision;
	return stream.str();
}

void VersionNumber::get(const std::string& str, VersionNumber& version) {
	std::string versionSubstr;
	size_t index = str.find_first_of(' ');
	if (index != std::string::npos) {
		versionSubstr = str.substr(0, index);
	}
	else {
		versionSubstr = str;
	}

	version.nVersionMajor = 0;
	index = versionSubstr.find_first_of('.');
	if (index != std::string::npos) {
		version.nVersionMajor = std::stol(versionSubstr.substr(0, index));
		versionSubstr = versionSubstr.substr(index + 1);
	}
	else if (!versionSubstr.empty()) {
		version.nVersionMajor = std::stol(versionSubstr);
		return;
	}

	version.nVersionMinor = 0;
	version.nVersionRevision = 0;
	index = versionSubstr.find_first_of('.');
	if (index != std::string::npos) {
		version.nVersionMinor = std::stol(versionSubstr.substr(0, index));
		versionSubstr = versionSubstr.substr(index + 1);
		index = versionSubstr.find_first_of('.');
		if (index != std::string::npos) {
			version.nVersionRevision = std::stol(versionSubstr.substr(0, index));
		}
		else if (!versionSubstr.empty()) {
			version.nVersionRevision = std::stol(versionSubstr);
		}
	}
	else if (!versionSubstr.empty()) {
		version.nVersionMinor = std::stol(versionSubstr);
	}
}
