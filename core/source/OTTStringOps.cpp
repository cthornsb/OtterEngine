#include "OTTStringOps.hpp"

#include <sstream>

constexpr char kLowercaseLow  = 'a';
constexpr char kLowercaseHigh = 'z';
constexpr char kUppercaseLow  = 'A';
constexpr char kUppercaseHigh = 'Z';

bool ott::StringIsNumeric(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if ((str[i] < '0' || str[i] > '9') && str[i] != '.')
			return false;
	return true;
}

bool ott::StringIsInteger(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if (str[i] < '0' || str[i] > '9')
			return false;
	return true;
}

bool ott::StringIsDecimal(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if ((str[i] < '0' || str[i] > '9') && str[i] != '.')
			return false;
	return true;
}

bool ott::StringIsHexadecimal(const std::string& str) {
	if (str.length() < 2 || str.find('$') == std::string::npos)
		return false;
	for (size_t i = 0; i < str.length(); i++) {
		if ((str[i] < '0' || str[i] > '9') && (str[i] < 'a' || str[i] > 'f') && (str[i] < 'A' || str[i] > 'F') && str[i] != '$')
			return false;
	}
	return true;
}

bool ott::StringIsBinary(const std::string& str) {
	if (str.length() < 2 || str.find('b') == std::string::npos)
		return false;
	for (size_t i = 0; i < str.length(); i++)
		if (str[i] != '0' && str[i] != '1' && str[i] != 'b')
			return false;
	return true;
}

bool ott::StringIsNotNumeric(const std::string& str) {
	return (!StringIsDecimal(str) && !StringIsHexadecimal(str) && !StringIsBinary(str));
}

short ott::TwosCompliment(const unsigned char& n) {
	// Compute the two's compliment of an unsigned byte
	if ((n & 0x80) == 0) // Positive value
		return (short)n;
	// Negative value
	return(((n & 0x007F) + (n & 0x0080)) - 0x100);
}

unsigned int ott::SplitString(const std::string& input, std::vector<std::string>& output, const unsigned char& delim/*='\t'*/) {
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

std::string ott::ExtractString(std::string& str, const char& c1, const char& c2, const std::string& repstr/*=""*/) {
	size_t index1 = str.find_last_of(c1);
	if (index1 == std::string::npos) {
		return "";
	}
	size_t index2 = str.find_first_of(c2, index1 + 1);
	if (index2 == std::string::npos) {
		return "";
	}
	std::string retval = str.substr(index1 + 1, index2 - (index1 + 1));
	str.replace(index1, retval.length() + 2, repstr);
	return retval;
}

unsigned int ott::OccurancesOfString(const std::string& str, const char& c) {
	unsigned int retval = 0;
	for (auto strc = str.begin(); strc != str.end(); strc++) {
		if (*strc == c)
			retval++;
	}
	return retval;
}

std::string ott::UCharToHex(const unsigned char& input) {
	std::stringstream stream;
	stream << std::hex << (int)input;
	std::string output = stream.str();
	if (output.length() < 2) output = "0" + output;
	return ("$" + output);
}

std::string ott::UShortToHex(const unsigned short& input) {
	std::stringstream stream;
	stream << std::hex << (int)input;
	std::string output = stream.str();
	output = (output.length() < 4 ? std::string(4 - output.length(), '0') : "") + output;
	return ("$" + output);
}

std::string ott::UCharToBinary(const unsigned char& input, const int& startBit/*=0*/) {
	std::stringstream stream;
	for (int i = 7; i >= startBit; i--)
		stream << ((input & (0x1 << i)) != 0);
	return stream.str();
}

std::string ott::UShortToBinary(const unsigned short& input, const int& startBit/*=0*/) {
	std::stringstream stream;
	for (int i = 15; i >= startBit; i--)
		stream << ((input & (0x1 << i)) != 0);
	return stream.str();
}

std::string ott::UShortToAscii(const unsigned short& input) {
	std::string retval;
	retval += (char)((input & 0x00ff));
	retval += (char)((input & 0xff00) >> 8);
	return retval;
}

std::string ott::UIntToAscii(const unsigned int& input) {
	std::string retval;
	retval += (char)((input & 0x000000ff));
	retval += (char)((input & 0x0000ff00) >> 8);
	retval += (char)((input & 0x00ff0000) >> 16);
	retval += (char)((input & 0xff000000) >> 24);
	return retval;
}

std::string ott::UCharToString(const unsigned char& input) {
	std::stringstream stream;
	stream << (int)input;
	return stream.str();
}

std::string ott::UShortToString(const unsigned short& input) {
	std::stringstream stream;
	stream << input;
	return stream.str();
}

std::string ott::UIntToString(const unsigned int& input) {
	std::stringstream stream;
	stream << input;
	return stream.str();
}

std::string ott::FloatToString(const float& input, const unsigned short& fixed/*=0*/) {
	std::stringstream stream;
	if (fixed != 0) {
		stream.precision(fixed);
		stream << std::fixed;
	}
	stream << input;
	return stream.str();
}

std::string ott::DoubleToString(const double& input, const unsigned short& fixed/*=0*/) {
	std::stringstream stream;
	if (fixed != 0) {
		stream.precision(fixed);
		stream << std::fixed;
	}
	stream << input;
	return stream.str();
}

std::string ott::StringToUppercase(const std::string& str) {
	std::string retval = str;
	for (size_t i = 0; i < str.length(); i++) {
		if (retval[i] >= kLowercaseLow && str[i] <= kLowercaseHigh)
			retval[i] = (str[i] - kLowercaseLow) + kUppercaseLow;
	}
	return retval;
}

std::string ott::StringToLowercase(const std::string& str) {
	std::string retval = str;
	for (size_t i = 0; i < str.length(); i++) {
		if (retval[i] >= kUppercaseLow && str[i] <= kUppercaseHigh)
			retval[i] = (str[i] - kUppercaseLow) + kUppercaseLow;
	}
	return retval;
}

std::string ott::StripWhitespace(const std::string& str) {
	return str.substr(0, str.find_last_not_of(' ') + 1);
}

std::string ott::StripAllWhitespace(const std::string& str) {
	std::string retval = "";
	for (auto c = str.begin(); c != str.end(); c++) {
		if (*c != ' ' && *c != '\n' && *c != '\t') {
			retval += *c;
		}
	}
	return retval;
}

bool ott::RemoveCharacter(std::string& str, const char& c) {
	size_t index = str.find(c);
	if (index != std::string::npos) {
		str.replace(index, 1, "");
		return true;
	}
	return false;
}

bool ott::RemoveAllCharacters(std::string& str, const char& c) {
	int counter = 0;
	while (RemoveCharacter(str, c)) {
		counter++;
	}
	return (counter != 0);
}

unsigned char ott::GetUserInputUChar(const std::string& str) {
	return (unsigned char)GetUserInputUShort(str);
}

unsigned short ott::GetUserInputUShort(const std::string& str) {
	return (unsigned short)GetUserInputUInt(str);
}

unsigned int ott::GetUserInputUInt(const std::string& str) {
	std::string input = str;
	if (StringIsHexadecimal(str)) { // Hex
		RemoveCharacter(input, '$');
		return stoul(input, 0, 16);
	}
	else if (StringIsBinary(str)) { // Binary
		RemoveCharacter(input, 'b');
		return stoul(input, 0, 2);
	}
	else if (StringIsNumeric(str)) { // Decimal
		return stoul(input, 0, 10);
	}
	return 0;
}
