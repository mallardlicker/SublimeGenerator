// main.cpp
// Author: Justin Bunting
// Created: 2024/08/06
// Last Modified: 2025/10/05 23:13

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

#include <CppUtils/CppUtils.h>

#include "TextFormatter.h"
#include "Project.h"
#include "FileOps.h"

#ifdef __APPLE__
    #include <libgen.h>
    #include <limits.h>
    #include <mach-o/dyld.h>
    #include <unistd.h>
#endif


// static int TEXTWIDTH = 100;
static std::string TITLE = "Project Generator"; 
// TODO: add MacOS Application types: 
// 1. implement new Enums/datastructures/generation and terms DONE
// 2. actually modify CMakeList.txt's and add the two new types

// TODO: implement generation of Windows .exe's in application generation (edit CMakeLists.txt's)

int main(int argc, char* argv[]) {
	
	system("tabs -4");
	CppUtils::CppUtils_Init();
	
	
	// Initialize variables
	Project p, oldP;
	
	// std::filesystem::path baseDir = File::getUserDirectory();
	// std::filesystem::path assetsDir = "/Users/mallardlicker/scripts/C++/SublimeProjects/SublimeGenerator/assets";
	
	std::string inputText = "$$";
	int inputNum;
	int currentParam;
	bool isValid = true;
	bool editingParam = false;
	
	bool projectExists = false;
	
	
	// ! Command line args
	// Either load project name and type from command line arguments, or prompt user for name
	bool doPrompt = true;
	if (argc > 0) {
		
		for (int i = 0; i < argc; i++) {
			
			std::string arg = argv[i];
			
			if (arg.find("pg") != std::string::npos) {
				continue;
			}
			
			if (arg.find('-') != std::string::npos) {
				
				// Type argument
				std::cout << ProjectInfo::strToPType(arg) << std::endl;
				p.type = ProjectInfo::strToPType(arg);
			}
			else {
				
				// Name argument
				p.name = arg;
				doPrompt = false;
			}
		}
	}
	if (doPrompt) {
		
		// Prompt user for new project name
		while (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) != inputText.end()) {
			system("clear");
			TextFormatter::printTitle(TITLE);
			std::cout << std::left << std::setw(TextFormatter::TEXT_WIDTH / 5 - 2) << "\tProject name: ";
			std::cin.clear();
			std::cin >> inputText;
		}
		
		p.name = inputText;
	}
	
	// ! Project editing input
	// determine whether a project already exists under that name, and load if so
	// important to remember that the only information we currently have would be the project name/type, meaning 
	//  only Name.prjGen, project/Name.prjGen, and Name/Project/Name.prjGen are the only "findable" locations of
	//  a prjGen file.
	std::filesystem::path prjGenFile = 	std::filesystem::exists(p.name + ".prjGen") ? p.name + ".prjGen" :
										std::filesystem::exists(std::string("project") + "/" + p.name + ".prjGen") ? std::string("project") + "/" + p.name + ".prjGen" :
										std::filesystem::exists(p.name + "/" + "project" + "/" + p.name + ".prjGen") ? p.name + "/" + "project" + "/" + p.name + ".prjGen" :
										"ThisFileDoesNotExist.pants";
	if (std::filesystem::exists(prjGenFile)) {
		
		ProjectInfo::importProjectInfo(p, prjGenFile);
		
		if (p.isGenerated) {
			
			oldP = p;
		}
	}
	else {
		
		// set generic (default) values
		ProjectInfo::fixFolderNames(p);
	}
	
	
	
	while (true) {
		
		
		if (!editingParam) {
			// ! Main prompt and information
			system("clear");
			TextFormatter::printProject(p, TITLE); // print all project information
			
			// invalid response message
			if (!isValid) {
				
				std::cout << "\n\tPrevious input invalid: " << inputText << std::endl;
			}
			
			// input prompt
			std::cout << "\n\t\tEnter number to change/inspect value, or (0) to " +
							std::string(p.isGenerated ? "edit" : "generate") + " project.\n\t\t\t-> ";
			std::cin.clear();
			std::cin >> std::setw(1) >> inputText >> std::setw(0);
			
			// validate input
			inputNum = int(inputText[0]) - int('0'); // has to be 0 and not EditParam::Build (to avoid triple-casting)
			currentParam = inputNum;
			isValid = true;
			editingParam = true;
			
			if (inputNum == EditParam::Build) {
				if (std::filesystem::exists(p.directory / p.name)) {
					
					std::cout << "\n\t\tProject folder already exists! Type 'OVERRIDE' to edit that project.\n\t\t\t-> ";
					std::cin.clear();
					std::cin >> std::setw(8) >> inputText >> std::setw(0);
					
					if (inputText == "OVERRIDE") {
						
						projectExists = true;
						break;
					}
				}
				else {
					break;
				}
			}
			else if ( (inputNum == EditParam::AppFolder && (p.type == PType::Basic || p.type == PType::Library)) || 
			     (inputNum == EditParam::LibFolder && (p.type == PType::Basic || p.type == PType::Standalone)) || 
			     inputNum > EditParam::LibFolder || inputNum < 0) {
				
				isValid = false;
				editingParam = false;
				continue;
			}
		}
		
		
		// ! Param editing and descriptions
		system("clear");
		TextFormatter::printProject(p, TITLE, currentParam);
		
		// invalid response message
		if (!isValid) {
			
			std::cout << "\n\tPrevious input invalid: " << inputText << std::endl;
		}
		
		// input prompt
		std::cout << std::endl;
		std::cout << "\t\tEnter new value, " <<
					std::string(currentParam == 4
									? "(f) to open selector, "
									: "")
					<< "or ("
					<< currentParam
					<< ") to return to main project page.\n\t\t\t-> ";
		std::cin.clear();
		std::cin >> inputText;
		
		// validate input
		inputNum = int(inputText[0]) - int('0'); // has to be 0 and not EditParam::Build (to avoid triple-casting)
		isValid = true;
		editingParam = false;
		
		if (inputNum == currentParam) {
			
			continue; // allows returning to main page
		}
		
		PType t = ProjectInfo::strToPType(inputText);
		std::filesystem::path res;
		switch (currentParam) {
			
			case EditParam::Name: // name -> changes all folder names & resets executable
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end()) {
					
					// as long as it is alphanumeric, allow it
					p.name = inputText;
					
					ProjectInfo::fixFolderNames(p);
					
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case EditParam::Type: // type -> changes all folder names & resets executable
				if (t != PType::None) {
					p.type = t;
					
					ProjectInfo::fixFolderNames(p);
					
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not one of the six types";
				}
				break;
				
			case EditParam::ExeName: // executable name
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end()) {
					
					// as long as it is alphanumeric, allow it
					p.exeName = inputText;
					
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case EditParam::Directory: // directory
				res = inputText;
				if (inputText[0] == 'f') {
					
					if (FileOps::openFolder(p.directory.string(), res)) {
						
						p.directory = res;
						continue;
					}
					else {
						
						inputText = "Failed to open folder";
					}
				}
				else if (std::filesystem::exists(res) && std::filesystem::is_directory(res)) { // is a valid directory name
					
					if (inputText[inputText.length()-1] == '/') {
						inputText = "Folder cannot end in '/'";
						break;
					}
					
					p.directory = inputText;
					continue;
				}
				else {
					
					inputText = "Invalid folder";
				}
				
				break;
			
			case EditParam::ProjectFolder: // project folder (5)
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end()) {
					
					// alphanumeric only
					p.projectFolder = inputText;
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case EditParam::AppFolder: // app folder
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end()) {
					
					// alphanumeric only
					p.appFolder = inputText;
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case EditParam::LibFolder: // lib folder
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end())
				{

					// alphanumeric only
					p.libFolder = inputText;
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
		}
		
		isValid = false;
		editingParam = true;
		
	} // while
	
	
	// ! actually generate project
	std::cout << "\n\t" << std::string(projectExists ? "Editing" : "Generating") << 
					" project '" << p.name << "' ..." << std::endl;
	
	// start with project folder
	std::filesystem::path projectDir = p.directory / p.projectFolder;
	std::filesystem::path assetsDir = "/Users/mallardlicker/scripts/C++/SublimeProjects/ProjectGenerator/ProjectGenerator/assets";
	assetsDir = assetsDir / ProjectInfo::pTypeToStr(p.type, true);
	CppUtils::File::createDirIfNotExists(projectDir);
	
	// spicy copy each file recursively from asset directory
	FileOps::spicyCopy(p, assetsDir, projectDir);
	// TODO: currently only generates projects, or generates over projects: implement editing (replacing terms) in projects
	// TODO: also implement moving of projects via changing of project directory
	
	std::cout << "\n\tDone." << std::endl;
	
	return 0;
}