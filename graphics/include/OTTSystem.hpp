#include <string>

namespace ott{
	std::string getCurrentWorkingDirectory();

	bool setCurrentWorkingDirectory(const std::string& path);

	std::string getExecutingPath();

	std::string getExecutingDirectory();
};