#pragma once
#include "common.h"

namespace UTILS {
	bool DirectoryExists(const std::string& dirPath);
	std::string GetWorkingDirectory();
	std::string GetMediaFolder();
	std::string operator+(const std::string& str0, const std::string& str1);
};