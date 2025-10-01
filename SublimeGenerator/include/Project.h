// Project.h
// -> Project struct, enum (PType), and ProjectInfo namespace/function definitions
// Author: Justin Bunting
// Created: 2025/10/01
// Last Modified: 2025/10/01 11:41

#ifndef _PROJECT_H
#define _PROJECT_H


#include <string>
#include <ostream>
#include <sstream>
#include <fstream>
#include <filesystem>

// ! MacOS Application types:
// -	Basic is an excecutable (-b)
// - 	Library is just an includable library generator (-l)
// - 	Standalone is an executable with libraries (-s)
// - 	Application is an (MacOS) app with libraries (-a)
// - 	StandaloneEngine is an executable that can use libraries containing a custom library (-se)
// - 	ApplicationEngine is an (MaxOS) app that can use libraries containing a custom library (-ae)

enum PType {
	Basic = 0, //				just cpp and include folders
	Library, //					library folder, library inclusion ability
	Standalone, // 				executable folder, library inclusion ability
	Application, // 			application folder, library inclusion ability, generates .app
	StandaloneEngine, //		executable folder (uses library) and library folder (with library inclusion)
	ApplicationEngine, //		application folder (uses library) and library folder (with library inclusion)
	
	None = -1
};

struct Project {
	std::string name = "Name";
	PType type = PType::Basic;
	
	std::string exeName = "executable";
	std::filesystem::path directory = std::filesystem::current_path();
	
	std::string projectFolder = "Folder";
	std::string appFolder = "N/A";
	std::string libFolder = "N/A";
	
	bool isGenerated = false;
};

namespace ProjectInfo {

	// Converts the string names of ProjectType enums to their associated enum values including
	//  coverage for the exact names (ie "Standalone & Engine"), generated data names (ie
	//  "StEngine"/"StandaloneEngine"), and command line flag names (ie "-se").
	static PType strToPType(const std::string& str) {
		
		if (str == "Basic" || str == "-b") {
			return PType::Basic;
		}
		if (str == "Library" || str == "-l") {
			return PType::Library;
		}
		if (str == "Standalone" || str == "-s") {
			return PType::Standalone;
		}
		if (str == "Application" || str == "-a") {
			return PType::Application;
		}
		if (str == "Standalone & Engine" || str == "StEngine" || str == "StandaloneEngine" || str == "-se") {
			return PType::StandaloneEngine;
		}
		if (str == "Application & Engine" || str == "AppEngine" || str == "ApplicationEngine" || str == "-ae") {
			return PType::ApplicationEngine;
		}
		
		return PType::None;
	}

	// Converts ProjectType enums into string representations, for either text display or
	//  settings saving (isFolder = true).
	static std::string pTypeToStr(const PType& type, const bool isFolder = false) {
		
		switch (type) {
			
			case PType::Basic:
				return "Basic";
				break;
			case PType::Library:
				return "Library";
				break;
			case PType::Standalone:
				return "Standalone";
				break;
			case PType::Application:
				return "Application";
				break;
			case PType::StandaloneEngine:
				if (isFolder) {
					return "StandaloneEngine";
				}
				return "Standalone & Engine";
				break;
			case PType::ApplicationEngine:
				if (isFolder) {
					return "ApplicationEngine";
				}
				return "Application & Engine";
				break;
			default:
				return "";
		}
	}

	static void fixFolderNames(Project &prj) {

		prj.exeName = "N/A";
		prj.projectFolder = prj.name;
		prj.appFolder = "N/A";
		prj.libFolder = "N/A";

		if (prj.type == PType::Standalone || prj.type == PType::ApplicationEngine)
		{
			prj.exeName = "executable";
			prj.appFolder = prj.name;
		}
		if (prj.type == PType::Library)
		{
			// want libraries to show up as the project name, and allow user to determine whether
			//  to add "Lib" in library name
			prj.libFolder = prj.name; // + "Lib";
		}
		if (prj.type == PType::ApplicationEngine)
		{
			prj.exeName = "executable";
			prj.libFolder = prj.name + "Engine";
		}
	}

	static void importProjectInfo(Project &prj, const std::filesystem::path sublPrjFile) {

		std::ifstream inputFile(sublPrjFile.string());

		if (inputFile.is_open())
		{

			std::string line;
			size_t pos;

			// bypass all lines in main file
			int count = 1;
			std::getline(inputFile, line); // skip first line: "{"
			while (std::getline(inputFile, line) && count > 0)
			{

				pos = line.find("{");

				while (pos != std::string::npos)
				{

					count++;
					pos = line.find("{", pos + 1);
				}

				pos = line.find("}");

				while (pos != std::string::npos)
				{

					count--;
					pos = line.find("}", pos + 1);
				}
			}

			// search for key terms in text
			while (std::getline(inputFile, line))
			{

				if (!prj.isGenerated)
				{
					pos = line.find("Project Name");

					if (pos != std::string::npos)
					{

						prj.isGenerated = true;
					}
				}

				if (prj.isGenerated)
				{

					pos = line.find("'");

					if (pos != std::string::npos)
					{

						std::string val = line.substr(pos + 1, line.find("'", pos + 1) - pos - 1);
						std::cout << val << std::endl;

						switch (count)
						{
						case 0:
							prj.name = val;
							break;
						case 1:
							prj.type = strToPType(val);
							break;
						case 2:
							prj.exeName = val;
							break;
						case 3:
							prj.directory = val;
							break;
						case 4:
							prj.projectFolder = val;
							break;
						case 5:
							prj.appFolder = val;
							break;
						case 6:
							prj.libFolder = val;
							break;
						}

						count++;
					}
				}
			}

			// unrecognized structure: default
			if (!prj.isGenerated)
			{

				// set generic (default) values
				fixFolderNames(prj);
			}

			inputFile.close();
		}
		else
		{

			// set generic (default) values
			fixFolderNames(prj);
		}
	}
	
	static std::string exportProjectInfo(const Project &prj) {

		std::string exportText;

		exportText = exportText + "// \n" +
					 "// SublimeGenerator (sg) Project Info\n" +
					 "// \n" +
					 "//  -> Run 'sg ProjectName' to modify these values:\n" +
					 "//   modification of string values that are equal may produce undesireable results.\n" +
					 "// \n" +
					 "// Project Name: '" + prj.name + "'\n" +
					 "// Type: '" + pTypeToStr(prj.type) + "'\n" +
					 "// Executable: '" + prj.exeName + "'\n" +
					 "// Directory: '" + prj.directory.string() + "'\n" +
					 "// Project Folder: '" + prj.projectFolder + "'\n" +
					 "// App Folder: '" + prj.appFolder + "'\n" +
					 "// Lib Folder: '" + prj.libFolder + "'\n" +
					 "// ";

		return exportText;
	}
}


#endif // _PROJECT_H