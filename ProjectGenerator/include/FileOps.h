// FileOps.h
// -> Functions which use the CppUtils::File system to open/close/modift files, as well as helper functions
// Author: Justin Bunting
// Created: 2025/10/01
// Last Modified: 2025/10/02 21:41

#ifndef _FILEOPS_H
#define _FILEOPS_H


#include <string>
#include <filesystem>

#include "Project.h"

namespace FileOps {
    
    bool openFolder(const std::string& initialPath, std::filesystem::path& fileResult);
    void spicyCopy(const Project& prj, const std::filesystem::path& fromFolder, const std::filesystem::path& toFolder);
    void searchReplace(std::string& str, std::string searchTerms[], std::string replaceTerms[], int numTerms);
    void searchReplacePrjTerms(std::string& str, const Project &prj);
}


#endif // _FILEOPS_H