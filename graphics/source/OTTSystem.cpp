#ifndef WIN32
#include <unistd.h>
#else
#include <direct.h>
#include <Windows.h>
#endif // ifndef WIN32

#include "OTTSystem.hpp"

namespace ott {
	/** Get the current working directory (CWD)
	  */
	std::string getCurrentWorkingDirectory() {
		char* str = _getcwd(0x0, 0);
		if (!str)
			return std::string();
		std::string retval(str);
		delete[] str;
		return retval;
	}

	/** Set the current working directory (CWD)
	  * @return True if the CWD is changed successfully
	  */
	bool setCurrentWorkingDirectory(const std::string& path) {
		return (_chdir(path.c_str()) == 0);
	}

	/** Get the full path of the currently executing program
	  */
	std::string getExecutingPath() {
		char str[1024];
		std::string path;
#ifndef WIN32
		if (readlink("/proc/self/exe", str, 1024) > 0) { // untested
			path = std::string(str);
		}
#else				
		if (GetModuleFileNameA(NULL, str, 1024) > 0) { // Get the full executable path
			path = std::string(str);
		}
#endif // ifndef WIN32
		return path;
	}

	/** Get the directory of the currently executing program
	  */
	std::string getExecutingDirectory() {
		std::string path = getExecutingPath();
#ifndef WIN32
		size_t index = path.find_last_of('/');
#else
		size_t index = path.find_last_of('\\');
#endif // ifndef WIN32
		std::string dir = ".";
		if (index != std::string::npos) {
			dir = path.substr(0, index);
		}
		return dir;
	}
};
