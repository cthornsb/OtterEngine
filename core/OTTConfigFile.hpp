#ifndef CORE_OTT_CONFIG_FILE_HPP
#define CORE_OTT_CONFIG_FILE_HPP

#include <string>
#include <map>

namespace ott {

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
	std::string CurrentFilename() const { 
		return filename; 
	}

	/** Get the name of the most recently searched variable
	  */
	std::string CurrentParameterName() const { 
		return currentName; 
	}

	/** Get the value of the most recently searched variable
	  */
	std::string CurrentParameterString() const { 
		return currentValue; 
	}

	/** Get the value of the most recently searched variable
	  */
	std::string Value(const std::string& name) const ;

	bool GetBoolean(const std::string& name) const;

	unsigned char GetUChar(const std::string& name) const ;

	unsigned short GetUShort(const std::string& name) const;

	unsigned int GetUInt(const std::string& name) const;

	float GetFloat(const std::string& name) const;

	double GetDouble(const std::string& name) const;

	std::string Value() const;

	bool GetBoolean() const;

	unsigned char GetUChar() const;

	unsigned short GetUShort() const;

	unsigned int GetUInt() const;

	float GetFloat() const;

	double GetDouble() const;

	void Print() const;

	/** Return an iterator to the beginning of the user input variable map
	  */
	std::map<std::string, std::string>::iterator Begin() { 
		return parameters.begin(); 
	}

	/** Return an iterator to the end of the user input variable map
	  */
	std::map<std::string, std::string>::iterator End() { 
		return parameters.end(); 
	}

private:
	bool bGood; ///< Set to true if user input read successfully

	std::string filename; ///< Full path to input config file

	std::string currentName; ///< Most recently searched variable name

	std::string currentValue; ///< Most recently searched variable value string

	std::map<std::string, std::string> parameters; ///< Map of user input variable names and string values
};

} /* namespace ott */

#endif // #ifndef CORE_OTT_CONFIG_FILE_HPP
