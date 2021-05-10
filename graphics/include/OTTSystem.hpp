#include <string>

namespace ott{
	/** Get the current working directory (CWD)
	  */
	std::string getCurrentWorkingDirectory();

	/** Set the current working directory (CWD)
	  * @return True if the CWD is changed successfully
	  */
	bool setCurrentWorkingDirectory(const std::string& path);

	/** Get the full path of the currently executing program
	  */
	std::string getExecutingPath();

	/** Get the directory of the currently executing program
	  */
	std::string getExecutingDirectory();
}
