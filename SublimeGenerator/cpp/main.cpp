// main.cpp
// Author: Justin Bunting
// Created: 2024/08/06
// Last Modified: 2025/10/01 11:59

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
static std::string TITLE = "Sublime Project Generator"; 
// TODO: port to be just a "project generator":
// 1. add project/.prjgen file which stores all the generation/project data
// 2. allow for selection of editor using project/ProjectName.(workspace)
//		-> add support for .sublime-workspace, .code-workspace (VSCode), and possibly more if other editors are used

// TODO: add MacOS Application types: (In-Progress)
// ! MacOS Application types:
// -	Basic is an excecutable (-b)
// - 	Library is just an includable library generator (-l)
// - 	Standalone is an executable with libraries (-s)
// - 	Application is an (MacOS) app with libraries (-a)
// - 	StandaloneEngine is an executable that can use libraries containing a custom library (-se)
// - 	ApplicationEngine is an (MaxOS) app that can use libraries containing a custom library (-ae)


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
	if (argc > 0) {
		
		for (int i = 0; i < argc; i++) {
			
			std::string arg = argv[i];
			
			if (arg == "sg") {
				continue;
			}
			
			if (arg.find('-') != std::string::npos) {
				
				// Type argument
				p.type = ProjectInfo::strToPType(arg); // TODO: test if working
			}
			else {
				
				// Name argument
				p.name = arg;
			}
		}
	}
	else {
		
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
	if (std::filesystem::exists(p.name + ".sublime-project") || std::filesystem::exists(p.name + "/" + p.name + ".sublime-project")) {
		
		std::filesystem::path sublPrjFile = std::filesystem::exists(p.name + ".sublime-project") ? p.name + ".sublime-project" :
																					p.name + "/" + p.name + ".sublime-project";
		
		ProjectInfo::importProjectInfo(p, sublPrjFile);
		
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
				
				std::cout << "\n\tInvalid input: " << inputText << std::endl;
			}
			
			// input prompt
			std::cout << "\n\t\tEnter number to change/inspect value, or (0) to " +
							std::string(p.isGenerated ? "edit" : "generate") + " project.\n\t\t\t-> ";
			std::cin.clear();
			std::cin >> std::setw(1) >> inputText >> std::setw(0);
			
			// validate input
			inputNum = int(inputText[0]) - int('0');
			currentParam = inputNum;
			isValid = true;
			editingParam = true;
			
			if (inputNum == 0) {
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
			else if ( (inputNum == 6 && (p.type == PType::Basic || p.type == PType::Library)) || 
			     (inputNum == 7 && (p.type == PType::Basic || p.type == PType::Standalone)) || 
			     inputNum > 7 || inputNum < 0) {
				
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
			
			std::cout << "\n\tInvalid input: " << inputText << std::endl;
		}
		
		// input prompt
		std::cout << std::endl;
		std::cout << "\t\tEnter new value, ";
		if (currentParam == 4) {
			
			std::cout << "(f) to open selector, ";
		}
		std::cout << "or (" << currentParam << ") to return to main project page.\n\t\t\t-> ";
		std::cin.clear();
		std::cin >> inputText;
		
		// validate input
		inputNum = int(inputText[0]) - int('0');
		isValid = true;
		editingParam = false;
		
		if (inputNum == currentParam) {
			
			continue; // allows returning to main page
		}
		
		PType t = ProjectInfo::strToPType(inputText);
		std::filesystem::path res;
		switch (currentParam) {
			
			case 1: // name -> changes all folder names & resets executable
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
				
			case 2: // type -> changes all folder names & resets executable
				if (t != PType::None) {
					p.type = t;
					
					ProjectInfo::fixFolderNames(p);
					
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not one of the four types";
				}
				break;
				
			case 3: // executable name
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end()) {
					
					// as long as it is alphanumeric, allow it
					p.exeName = inputText;
					
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case 4: // directory
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
			
			case 5: // project folder
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end()) {
					
					// alphanumeric only
					p.projectFolder = inputText;
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case 6: // app folder
				if (find_if(inputText.begin(), inputText.end(), TextFormatter::isNotAlnumSpace) == inputText.end()) {
					
					// alphanumeric only
					p.appFolder = inputText;
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case 7: // lib folder
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
	std::filesystem::path assetsDir = "/Users/mallardlicker/scripts/C++/SublimeProjects/SublimeGenerator/SublimeGenerator/assets";
	assetsDir = assetsDir / ProjectInfo::pTypeToStr(p.type, true);
	CppUtils::File::createDirIfNotExists(projectDir);
	
	// spicy copy each file recursively from asset directory
	FileOps::spicyCopy(p, assetsDir, projectDir);
	// TODO: currently only generates projects, or generates over projects: implement editing (replacing terms) in projects
	// TODO: also implement moving of projects via changing of project directory
	
	std::cout << "\n\tDone." << std::endl;
	
	return 0;
}