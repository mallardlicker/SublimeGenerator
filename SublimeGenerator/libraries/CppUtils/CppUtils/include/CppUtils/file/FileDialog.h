#ifndef CPPUTILS_FILE_DIALOG_H
#define CPPUTILS_FILE_DIALOG_H

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#define NFD_NATIVE
#include <nfd.h>

namespace CppUtils {
	
	namespace FileDialog {
		
		bool openFile(const std::string& initialPath, std::filesystem::path& fileResult, 
					   std::vector<std::pair<std::string, std::string>> extensionFilters = { { "Default", "txt" } });
		
		// todo: implement me
		// bool openFiles(const std::string& initialPath, std::filesystem::path* fileResults, int& numResults, 
		//                std::vector<std::pair<std::string, std::string>> extensionFilters = { { "Default", "txt" } });
		
		bool openFolder(const std::string& initialPath, std::filesystem::path& fileResult);
		
		bool saveFile(const std::string& initialPath, std::filesystem::path& fileResult, 
					   std::vector<std::pair<std::string, std::string>> extensionFilters = { { "Default", "txt" } },
					   std::string extToAppend = ""); // append extensions have same form as extension filters: no 'dot'
		
		// void displayMessageBox(const std::string& title, const std::string& message);
	}
}

#endif