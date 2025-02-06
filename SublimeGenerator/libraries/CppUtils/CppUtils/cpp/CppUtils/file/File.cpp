#include "CppUtils/file/File.h"
#include "CppUtils/CppUtils.h"

#include <sys/stat.h>

#ifdef __APPLE__
    #include <libgen.h>
    #include <limits.h>
    #include <mach-o/dyld.h>
    #include <unistd.h>
#endif

namespace CppUtils {
	
	namespace File {
		
		FileHandle* openFile(const std::filesystem::path& filename) {
			
			FileHandle* file = new FileHandle();
			file->filename = filename.string();
			
			// open file only for reading input
			std::fstream tmpFile(file->filename.c_str(), std::ios::in | std::ios::binary);
			
			if (tmpFile.is_open()) {
				file->open = true;
				
				tmpFile.seekg(0, std::ios::end);
				file->size = tmpFile.tellg();
				
				file->data = new char[file->size];
				
				tmpFile.seekg(0, std::ios::beg);
				tmpFile.read(file->data, file->size);
				
				tmpFile.close();
			}
			else {
				file->open = false;
				file->size = 0;
				file->data = nullptr;
			}
			
			return file;
		}
		
		void closeFile(FileHandle* file) {
			
			if (file) {
				if (file->data) {
					delete[] file->data;
				}
				// else {
				// 	Logger::Warning("Tried to free invalid file.");
				// }
				delete file;
			}
			else {
				Logger::Warning("Tried to free invalid file.");
			}
		}
		
		std::string readFile(const std::filesystem::path& filename) {
			
			return readFile(filename.string().c_str());
		}
		
		std::string readFile(const char* filename) {
			
			std::string result;
			std::fstream tmpFile(filename, std::ios::in | std::ios::binary);
			
			if (tmpFile) {
				tmpFile.seekg(0, std::ios::end);
				result.resize(tmpFile.tellg());
				tmpFile.seekg(0, std::ios::beg);
				tmpFile.read(&result[0], result.size());
				tmpFile.close();
			}
			else {
				Logger::Error("Unable to read file %s .", filename);
			}
			
			return result;
		}
		
		bool writeFile(const char* data, const std::filesystem::path& filename) {
			
			std::ofstream outStream(filename);
			outStream << data;
			outStream.close();
			return true;
		}
		
		bool createFile(const std::filesystem::path& filename, const char* extToAppend) {
			
			std::filesystem::path fileToWrite = filename;
			
			if (!filename.has_extension()) {
				fileToWrite = filename.string() + extToAppend;
			}
			
			std::ofstream newFile(fileToWrite);
			bool res = newFile.is_open();
			newFile.close();
			
			return res;
		}
		
		bool deleteFile(const std::filesystem::path& filename) {
			try {
				if (std::filesystem::remove(filename)) {
					return true;
				}
			}
			catch(const std::filesystem::filesystem_error& err) {
				Logger::Error("Failed to delete: %s", err.what());
			}
			
			return false;
		}
		
		bool copyFile(const std::filesystem::path& fileToCopy, const std::filesystem::path& newFileLocation, const std::string& newFilename) {
			
			std::string newFileNameWithExt = newFilename.length() == 0 ? fileToCopy.filename().string() :
																		newFilename + fileToCopy.extension().string();
			
			std::filesystem::path newFilePath = newFileLocation / newFileNameWithExt;
			
			try {
				std::filesystem::copy_file(fileToCopy, newFilePath);
				return true;
			}
			catch(std::filesystem::filesystem_error& err) {
				Logger::Error("Failed to copy: %s", err.what());
			}
			
			return false;
		}
		
		std::filesystem::path getCwd() {
			
			// filesystem does all of the work (including OS-specific pathing)
			return std::filesystem::current_path();
		}
		
		std::filesystem::path getUserDirectory() {
			
			#ifdef __APPLE__
			
			std::string username = getenv("USER");
			return std::filesystem::path("/Users")/username;
			
			#endif
		}
		
		std::filesystem::path getSpecialAppFolder() {
			
			#ifdef __APPLE__
			
			// AppData is within Application Support
			// User preferences typically stored in Library/Preferences
			
			return getUserDirectory()/"Library"/"Application Support";
			
			#endif
		}
		
		// std::filesystem::path getExecutableDirectory() {
			
		// 	#ifdef __APPLE__
			
		// 	// todo: fix this!!
			
		// 	char rawPathName[PATH_MAX];
		// 	char realPathName[PATH_MAX];
		// 	uint32 rawPathSize = (uint32)sizeof(rawPathName);
			
		// 	if (!_NSGetExecutablePath(rawPathName, &rawPathSize)) {
				
		// 		realpath(rawPathName, realPathName);
		// 	}
			
		// 	return std::filesystem::path(realPathName).parent_path();
			
		// 	#endif
			
		// }
		
		std::vector<std::filesystem::path> getFilesInDir(const std::filesystem::path& directory) {
			if (!isDirectory(directory)) {
				Logger::Error("Input is not a directory: %s .", directory.c_str());
			}
			
			std::vector<std::filesystem::path> res;
			for (auto& p : std::filesystem::directory_iterator(directory)) {
				if (isFile(p.path())) {
					res.push_back(p.path());
				}
			}
			
			return res;
		}
		
		std::vector<std::filesystem::path> getFoldersInDir(const std::filesystem::path& directory) {
			if (!isDirectory(directory)) {
				Logger::Error("Input is not a directory: %s .", directory.c_str());
			}
			
			std::vector<std::filesystem::path> res;
			for (auto& p : std::filesystem::directory_iterator(directory)) {
				if (isDirectory(p.path())) {
					res.push_back(p.path());
				}
			}
			
			return res;
		}
		
		bool createDirIfNotExists(const std::filesystem::path& directory) {
			
			if (!std::filesystem::exists(directory)) {
				std::filesystem::path parent = directory.parent_path();
				
				createDirIfNotExists(parent);
				
				std::filesystem::create_directory(directory);
			}
			
			return true;
		}
		
		bool isFile(const std::filesystem::path& filepath) {
			if (!std::filesystem::is_regular_file(filepath)) {
				return false;
			}
			
			// backup option:
			struct stat sb;
			return stat(filepath.string().c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR);
		}
		
		// bool isHidden(const std::filesystem::path& filepath);
		
		bool isDirectory(const std::filesystem::path& directory) {
			if (std::filesystem::exists(directory)) {
				return std::filesystem::is_directory(directory);
			}
			
			// backup option:
			struct stat sb;
			return stat(directory.string().c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR);
			
			// return directory.string().find('.') == std::string::npos;
		}
		
		// std::filesystem::path getAbsolutePath(const std::filesystem::path& path) {
		// 	return std::filesystem::absolute(path);
		// }
		
	}
}