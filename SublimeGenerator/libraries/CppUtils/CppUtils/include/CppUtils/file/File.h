#ifndef CPPUTILS_FILE_H
#define CPPUTILS_FILE_H

#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace CppUtils {
	
	struct FileHandle {
		std::string filename = "";
		char* data = nullptr;
		uint32_t size = 0;
		bool open = false;
	};
	
	namespace File {
		
		FileHandle* openFile(const std::filesystem::path& filename);
		void closeFile(FileHandle* file);
		
		std::string readFile(const std::filesystem::path& filename);
		std::string readFile(const char* filename);
		bool writeFile(const char* data, const std::filesystem::path& filename);
		
		bool createFile(const std::filesystem::path& filename, const char* extToAppend = "");
		bool deleteFile(const std::filesystem::path& filename);
		bool copyFile(const std::filesystem::path& fileToCopy, const std::filesystem::path& newFileLocation, const std::string& newFilename = "");
		
		// implementations based on: https://stackoverflow.com/a/60250581
		std::filesystem::path getCwd();
		std::filesystem::path getUserDirectory();
		std::filesystem::path getSpecialAppFolder();
		// std::filesystem::path getExecutableDirectory();
		
		std::vector<std::filesystem::path> getFilesInDir(const std::filesystem::path& directory);
		std::vector<std::filesystem::path> getFoldersInDir(const std::filesystem::path& directory);
		bool createDirIfNotExists(const std::filesystem::path& directory);
		
		bool isFile(const std::filesystem::path& filepath);
		// bool isHidden(const std::filesystem::path& filepath);
		bool isDirectory(const std::filesystem::path& directory);
		
		// std::filesystem::path getAbsolutePath(const std::filesystem::path& path);
	}
}

#endif