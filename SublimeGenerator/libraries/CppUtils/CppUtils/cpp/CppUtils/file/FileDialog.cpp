#include "CppUtils/file/FileDialog.h"
#include "CppUtils/CppUtils.h"

namespace CppUtils {
	
	namespace FileDialog {
		
		bool openFile(const std::string& initialPath, std::filesystem::path& fileResult, 
							 std::vector<std::pair<std::string, std::string>> extensionFilters) {
			
			int numFilters = extensionFilters.size();
			nfdfilteritem_t filterItems[numFilters];
			
			for (int i = 0; i < numFilters; i++) {
				
				filterItems[i] = { extensionFilters[i].first.c_str(), extensionFilters[i].second.c_str() };
			}
			
			nfdchar_t *outPath;
			nfdresult_t nfdResult = NFD_OpenDialog(&outPath, filterItems, numFilters, initialPath.c_str());
			
			if (nfdResult == NFD_OKAY) {
				fileResult = outPath;
				Logger::Info("Resulting open file value: '%s'.", fileResult.string().c_str());
				NFD_FreePath(outPath);
				return true;
			}
			else if (nfdResult == NFD_CANCEL) {
				
				// user pressed cancel
				return false;
			}
			
			Logger::Error("While opening file: %s", NFD_GetError());
			return false;
		}
		
		bool openFolder(const std::string& initialPath, std::filesystem::path& fileResult) {
			
			nfdchar_t *outPath;
			nfdresult_t nfdResult = NFD_PickFolder(&outPath, initialPath.c_str());
			
			if (nfdResult == NFD_OKAY) {
				
				fileResult = outPath;
				Logger::Info("Resulting open folder value: '%s'.", fileResult.string().c_str());
				NFD_FreePath(outPath);
				return true;
			}
			else if (nfdResult == NFD_CANCEL) {
				
				// user pressed cancel
				return false;
			}
			
			Logger::Error("While opening folder: %s", NFD_GetError());
			return false;
		}
		
		bool saveFile(const std::string& initialPath, std::filesystem::path& fileResult, 
							 std::vector<std::pair<std::string, std::string>> extensionFilters,
							 std::string extToAppend) {
			
			int numFilters = extensionFilters.size();
			nfdfilteritem_t filterItems[numFilters];
			
			for (int i = 0; i < numFilters; i++) {
				
				filterItems[i] = { extensionFilters[i].first.c_str(), extensionFilters[i].second.c_str() };
			}
			
			nfdchar_t *outPath;
			nfdresult_t nfdResult = NFD_SaveDialog(&outPath, filterItems, numFilters, initialPath.c_str(), NULL);
			
			if (nfdResult == NFD_OKAY) {
				fileResult = outPath;
				
				if (fileResult.extension() == std::filesystem::path("")) {
					
					fileResult = fileResult.string() + "." + extToAppend;
				}
				
				NFD_FreePath(outPath);
				return true;
			}
			else if (nfdResult == NFD_CANCEL) {
				
				// user pressed cancel
				return false;
			}
			
			Logger::Error("While saving file: %s", NFD_GetError());
			return false;
		}
		
		// void displayMessageBox(const std::string& title, const std::string& message) {
			
			
		// }
	}
}