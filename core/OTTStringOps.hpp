#ifndef CORE_OTT_STRING_OPS_HPP
#define CORE_OTT_STRING_OPS_HPP

#include <vector>
#include <string>

namespace ott {

/** Return true if an input string is numerical
  */
bool StringIsNumeric(const std::string& str);

/** Return true if an input string is numerical and is an integer
  */
bool StringIsInteger(const std::string& str);

/** Return true if an input string is numerical and is a decimal value
  */
bool StringIsDecimal(const std::string& str);

/** Return true if an input string is numerical and is a hexadecimal value
  */
bool StringIsHexadecimal(const std::string& str);

/** Return true if an input string is numerical and is a binary value
  */
bool StringIsBinary(const std::string& str);

/** Return true if an input string is NOT numerical
  */
bool StringIsNotNumeric(const std::string& str);

/** Compute the two's compliment of an unsigned byte
  */
short TwosCompliment(const unsigned char& n);

/** Split an input string into parts about a specified delimiter character
  * @param input Input string to split into parts
  * @param output Vector of strings to store split strings
  * @param delim Delimiter character about which the input string will be split
  * @return The number of output strings the input was split into
  */
unsigned int SplitString(const std::string& input, std::vector<std::string>& output, const unsigned char& delim = '\t');

/** Extract a sub-string from an input string
  */
std::string ExtractString(std::string& str, const char& c1, const char& c2, const std::string& repstr = "");

/** Counter the number of occurances of character 'c' in the input string and return the result
  */
unsigned int OccurancesOfString(const std::string& str, const char& c);

/** Convert input integer to a hexadecimal string
  */
std::string UCharToHex(const unsigned char& input);

/** Convert input integer to a hexadecimal string
  */
std::string UShortToHex(const unsigned short& input);

/** Convert input integer to a binary string
  */
std::string UCharToBinary(const unsigned char& input, const int& startBit = 0);

/** Convert input integer to a binary string
  */
std::string UShortToBinary(const unsigned short& input, const int& startBit = 0);

/** Convert input integer to ascii string
  */
std::string UShortToAscii(const unsigned short& input);

/** Convert input integer to ascii string
  */
std::string UIntToAscii(const unsigned int& input);

/** Convert input integer to a decimal string
  */
std::string UCharToString(const unsigned char& input);

/** Convert input integer to a decimal string
  */
std::string UShortToString(const unsigned short& input);

/** Convert input integer to a decimal string
  */
std::string UIntToString(const unsigned int& input);

/** Convert input floating point to a string
  * If parameter 'fixed' is specified, fixed decimal point will be used
  */
std::string FloatToString(const float& input, const unsigned short& fixed = 0);

/** Convert input double to a string
  * If parameter 'fixed' is specified, fixed decimal point will be used
  */
std::string DoubleToString(const double& input, const unsigned short& fixed = 0);

/** Convert input alpha-numerical string to uppercase
  */
std::string StringToUppercase(const std::string& str);

/** Convert input alpha-numerical string to lowercase
  */
std::string StringToLowercase(const std::string& str);

/** Strip trailing whitespace characters from an input string
  */
std::string StripWhitespace(const std::string& str);

/** Strip ALL whitespace characters from an input string
  */
std::string StripAllWhitespace(const std::string& str);

/** Remove the first occurance of a specified character from an input string
  * @return True if a character was removed from the input string
  */
bool RemoveCharacter(std::string& str, const char& c);

/** Remove ALL occurances of a specified character from an input string
  * @return True if at least one character was removed from the input string
  */
bool RemoveAllCharacters(std::string& str, const char& c);

/** Get an unsigned 8-bit integer from a user input string
  * Acceptable input formats are decimal, hexadecimal, and binary (e.g. 1234, $abcd, and b0110)
  * @return The integer converted from the input string or 0 in the event that the input string was not numerical
  */
unsigned char GetUserInputUChar(const std::string& str);

/** Get an unsigned 16-bit integer from a user input string
  * Acceptable input formats are decimal, hexadecimal, and binary (e.g. 1234, $abcd, and b0110)
  * @return The integer converted from the input string or 0 in the event that the input string was not numerical
  */
unsigned short GetUserInputUShort(const std::string& str);

/** Get an unsigned 32-bit integer from a user input string
  * Acceptable input formats are decimal, hexadecimal, and binary (e.g. 1234, $abcd, and b0110)
  * @return The integer converted from the input string or 0 in the event that the input string was not numerical
  */
unsigned int GetUserInputUInt(const std::string& str);

} /* namespace ott */

#endif // #ifndef CORE_OTT_STRING_OPS_HPP
