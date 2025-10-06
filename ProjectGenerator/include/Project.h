// Project.h
// -> Project struct, enum (PType), and ProjectInfo namespace/function definitions
// Author: Justin Bunting
// Created: 2025/10/01
// Last Modified: 2025/10/05 21:44

#ifndef _PROJECT_H
#define _PROJECT_H


#include <string>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ! MacOS/Generator Application types:
// -	Basic is an excecutable (-b)
// - 	Library is just an includable library generator (-l)
// - 	Standalone is an executable with libraries (-s)
// - 	Application is an (MacOS) app with libraries (-a)
// - 	StandaloneEngine is an executable that can use libraries containing a custom library (-se)
// - 	ApplicationEngine is an (MaxOS) app that can use libraries containing a custom library (-ae)

//		Project Type		Executable?		cpp+incl in base dir?	Application?	Gens Library?	Uses Ext Libs?	Uses Custom (Int) Lib?
//								
//		Basic				Yes				Yes						No				No				No				No
//		Library				No				No						No				Yes				Yes				No
//		Standalone			Yes				No						No				No				Yes				No
//		Application			No				No						Yes				No				Yes				No
//		StandaloneEngine	Yes				No						No				Yes				Yes				Yes
//		ApplicationEngine	No				No						Yes				Yes				Yes				Yes

enum PType {
	Basic = 0,
	Library,
	Standalone,
	Application,
	StandaloneEngine,
	ApplicationEngine,
	
	None = -1
};

struct Project {
	std::string name = "Name"; // Project name
	PType type = PType::Basic;
	
	std::string exeName = name + "Standalone";
	std::filesystem::path directory = std::filesystem::current_path(); // the directory of the the project, typically .../desiredLocation/projectFolder/
	
	std::string projectFolder = name; // Folder containing the project
	std::string appFolder = "N/A"; // Folder containing cpp/include in an app/exe project
	std::string libFolder = "N/A"; // Folder containing cpp/include in/for a lib
	
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

		prj.exeName = prj.name + "Standalone";
		prj.projectFolder = prj.name;
		prj.appFolder = "N/A";
		prj.libFolder = "N/A";
		
		// executable only needs to be modified for App implementations
		if (prj.type == PType::Application || prj.type == PType::ApplicationEngine) {
			prj.exeName = prj.name; // + "App";
		}
		
		// library folder name
		if (prj.type == PType::Library) {
			// want libraries to show up as the project name, and allow user to determine whether
			//  to add "Lib" in library name
			prj.libFolder = prj.name; // + "Lib";
		}
		
		if (prj.type == PType::ApplicationEngine || prj.type == PType::StandaloneEngine) {
			prj.libFolder = prj.name + "Engine";
		}
		
		// app folder name for all standalones/apps
		if (prj.type == PType::Application || prj.type == PType::Standalone ||
			prj.type == PType::ApplicationEngine || prj.type == PType::StandaloneEngine) {
			
			prj.appFolder = prj.name;
		}
	}

	static void importProjectInfo(Project &prj, const std::filesystem::path projectGenerationFile) {
		
		std::ifstream inputFile(projectGenerationFile.string());

		if (inputFile.is_open())
		{
			try {
				
				json obj;
				inputFile >> obj;
				
				// Load data from JSON
				if (obj.contains("name")) 			{ 	prj.name = obj["name"]; }
				if (obj.contains("type"))			{	prj.type = strToPType(obj["type"]); }
				if (obj.contains("exeName"))		{	prj.exeName = obj["exeName"]; }
				if (obj.contains("directory"))		{	prj.directory = std::string(obj["directory"]); }
				if (obj.contains("projectFolder"))	{	prj.projectFolder = obj["projectFolder"]; }
				if (obj.contains("appFolder"))		{	prj.appFolder = obj["appFolder"]; }
				if (obj.contains("libFolder"))		{	prj.libFolder = obj["libFolder"]; }
				
				prj.isGenerated = true;
			}
			catch (const json::exception& e) {
				std::cerr << "Error parsing JSON: " << e.what() << std::endl;
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

		json obj;

		obj["name"] = prj.name;
		obj["type"] = pTypeToStr(prj.type);
		obj["exeName"] = prj.exeName;
		obj["directory"] = prj.directory.string();
		obj["projectFolder"] = prj.projectFolder;
		obj["appFolder"] = prj.appFolder;
		obj["libFolder"] = prj.libFolder;
		
		// Add metadata
        obj["_metadata"] = {
            {"generator", "ProjectGenerator (pg)"},
            {"version", "0.1.0"},
            {"description", "Run 'pg ProjectName' to modify these values"}
        };

		return obj.dump(4); // uses 4 spaces for pretty printing
	}
}


#endif // _PROJECT_H