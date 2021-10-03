#ifndef OTT_CONFIG_FILE_HPP
#define OTT_CONFIG_FILE_HPP

#include <sstream>
#include <string>
#include <map>

namespace ott {
	/** Return true if an input string is numerical
	  */
	bool isNumeric(const std::string& str);

	/** Convert input alpha-numerical string to uppercase
	  */
	std::string toUppercase(const std::string& str);

	/** Convert input alpha-numerical string to lowercase
	  */
	std::string toLowercase(const std::string& str);

	/** Split an input string into parts about a specified delimiter character
	  * @param input Input string to split into parts
	  * @param output Vector of strings to store split strings
	  * @param delim Delimiter character about which the input string will be split
	  * @return The number of output strings the input was split into
	  */
	unsigned int splitString(const std::string& input, std::vector<std::string>& output, const unsigned char& delim = '\t');
}

class ConfigFile {
public:
	/** Default constructor
	  */
	ConfigFile();

	/** Config filename constructor
	  */
	ConfigFile(const std::string& fname);

	/** Destructor
	  */
	~ConfigFile() {
	}

	/** Return true if user input was read successfully
	  */
	bool good() const { 
		return bGood; 
	}

	/** Read user input from a config file
	  */
	bool read(const std::string& fname);

	/** Search all user input variable with a given name
	  * @param bRequiredArg If set, 
	  * @return True 
	  */
	bool search(const std::string& name, bool bRequiredArg = false);

	/** Search for a true / false boolean flag with a given name
	  */
	bool searchBoolFlag(const std::string& name);

	/** Get the path to the input config file
	  */
	std::string getCurrentFilename() const { 
		return filename; 
	}

	/** Get the name of the most recently searched variable
	  */
	std::string getCurrentParameterName() const { 
		return currentName; 
	}

	/** Get the value of the most recently searched variable
	  */
	std::string getCurrentParameterString() const { 
		return currentValue; 
	}

	/** Get the value of the most recently searched variable
	  */
	std::string getValue(const std::string& name) const ;

	bool getBoolFlag(const std::string& name) const;

	unsigned char getUChar(const std::string& name) const ;

	unsigned short getUShort(const std::string& name) const;

	unsigned int getUInt(const std::string& name) const;

	float getFloat(const std::string& name) const;

	double getDouble(const std::string& name) const;

	std::string getValue() const;

	bool getBoolFlag() const;

	unsigned char getUChar() const;

	unsigned short getUShort() const;

	unsigned int getUInt() const;

	float getFloat() const;

	double getDouble() const;

	void print() const;

	/** Return an iterator to the beginning of the user input variable map
	  */
	std::map<std::string, std::string>::iterator begin() { 
		return parameters.begin(); 
	}

	/** Return an iterator to the end of the user input variable map
	  */
	std::map<std::string, std::string>::iterator end() { 
		return parameters.end(); 
	}

private:
	bool bGood; ///< Set to true if user input read successfully

	std::string filename; ///< Full path to input config file

	std::string currentName; ///< Most recently searched variable name

	std::string currentValue; ///< Most recently searched variable value string

	std::map<std::string, std::string> parameters; ///< Map of user input variable names and string values
};

#endif
