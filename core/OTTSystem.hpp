#ifndef CORE_OTT_SYSTEM_HPP
#define CORE_OTT_SYSTEM_HPP

#include <string>

namespace ott{

/** Get the current working directory (CWD)
	*/
std::string CurrentWorkingDirectory();

/** Set the current working directory (CWD)
	* @return True if the CWD is changed successfully
	*/
bool SetCurrentWorkingDirectory(const std::string& path);

/** Get the full path of the currently executing program
	*/
std::string ExecutingPath();

/** Get the directory of the currently executing program
	*/
std::string ExecutingDirectory();

/** Get the local system path for external assets and media files
	* @param filename Relative path of requested file. If not specified, full path to the assets directory will be returned.
	*/
std::string AssetsPath(const std::string& filename = "");

} /* namespace ott */

#endif // #ifndef CORE_OTT_SYSTEM_HPP
