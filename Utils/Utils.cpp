#include "Utils.h"
#include <direct.h>

namespace UTILS {

	bool DirectoryExists(const std::string& dirPath) {
		DWORD attributes = GetFileAttributesA(dirPath.c_str());
		if (attributes == INVALID_FILE_ATTRIBUTES)
			return false;
		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;
		return false;
	}

	std::string GetWorkingDirectory() {
		std::string workingDir;
		workingDir.resize(FILENAME_MAX);
		_getcwd(&workingDir.front(), workingDir.size());
		workingDir = std::string(workingDir.c_str());
		return workingDir;
	}

	std::string GetMediaFolder() {

		if (DirectoryExists("Media/")) {
			return GetWorkingDirectory() + std::string("/Media/");
		}
		if (DirectoryExists("../Media")) {
			return GetWorkingDirectory() + std::string("/../Media/");
		}
		throw std::runtime_error("Failed to find media folder");
	}	
};