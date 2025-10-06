// FileOps.cpp
// Author: Justin Bunting
// Created: 2025/10/01
// Last Modified: 2025/10/05 23:02

#include "FileOps.h"


#include <CppUtils/CppUtils.h>

#include "TextFormatter.h"

namespace FileOps {
    
    bool openFolder(const std::string& initialPath, std::filesystem::path& fileResult) {

        nfdchar_t *outPath;
        nfdresult_t nfdResult = NFD_PickFolder(&outPath, initialPath.c_str());

        if (nfdResult == NFD_OKAY) {

            fileResult = outPath;
            // Logger::Info("Resulting open folder value: '%s'.",
            // fileResult.string().c_str());
            NFD_FreePath(outPath);
            return true;
        } else if (nfdResult == NFD_CANCEL) {

            // user pressed cancel
            return false;
        }

        // Logger::Error("While opening folder: %s", NFD_GetError());
        return false;
    }
    
    void spicyCopy(const Project& prj, const std::filesystem::path& fromFolder, const std::filesystem::path& toFolder) {

        // create all folders
        for (std::filesystem::path &p : CppUtils::File::getFoldersInDir(fromFolder)) {

            std::string folderName = p.filename();

            // replace specified terms in folder name
            searchReplacePrjTerms(folderName, prj);

            CppUtils::File::createDirIfNotExists(toFolder / folderName);
            spicyCopy(prj, p, toFolder / folderName);
        }

        // copy, then modify each file (to add project info)
        for (std::filesystem::path &p : CppUtils::File::getFilesInDir(fromFolder)) {

            std::string filename = p.stem();

            // skip unnecessary files
            if (filename == ".DS_Store") {
                continue;
            }

            // replace specified terms in filename if necessary
            searchReplacePrjTerms(filename, prj);

            // copy to new directory and edit at the same time
            std::filesystem::path newFile = toFolder / (filename + p.extension().string());

            // open file to copy and new (copy) file
            std::ifstream inputFile(p.string());
            std::ofstream outputFile(newFile.string());

            if (inputFile.is_open() && outputFile.is_open()) {
                
                // save project information to .prjGen file
                if (newFile.extension() == ".prjGen") {
                    
                    // can ignore all contents of the inputFile, since we are saving current JSON to outputFile
                    outputFile << ProjectInfo::exportProjectInfo(prj);
                }
                else {
                    std::string line;
                    
                    while (std::getline(inputFile, line)) {

                        // replace specified terms in line if necessary
                        searchReplacePrjTerms(line, prj);

                        outputFile << line << "\n";
                    }
                }

                inputFile.close();
                outputFile.close();
            }
        }
    }
    
    
    void searchReplace(std::string& str, std::string searchTerms[], std::string replaceTerms[], int numTerms) {

        size_t index;

        // loop through each search term
        for (int i = 0; i < numTerms; i++) {

            // find and replace all instances of search term in string
            index = str.find(searchTerms[i]);

            while (index != std::string::npos) {

                str.replace(index, searchTerms[i].length(), replaceTerms[i]);
                index = str.find(searchTerms[i], index + replaceTerms[i].length());
            }
        }
    }
    
    void searchReplacePrjTerms(std::string &str, const Project &prj) {
        
        std::string appFolderCaps = prj.appFolder;
        std::transform(appFolderCaps.begin(), appFolderCaps.end(),
                        appFolderCaps.begin(), ::toupper);
        std::string libFolderCaps = prj.libFolder;
        std::transform(libFolderCaps.begin(), libFolderCaps.end(),
                        libFolderCaps.begin(), ::toupper);

		// use updated pg (not sg) names to find/replace
		std::string searchWords[] = {"pgPath",    "pgName",    "pgAppOrStlName", 
									"PGAPPORSTLNAME", "pgAppOrStl", "pgLibName", 
                                    "PGLIBNAME"};
        std::string replaceWords[] = {prj.directory / prj.projectFolder,
                                    prj.name,
                                    prj.appFolder,
                                    appFolderCaps,
                                    prj.exeName,
                                    prj.libFolder,
                                    libFolderCaps};
        
        searchReplace(str, searchWords, replaceWords, 7);
    }
}