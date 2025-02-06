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
using namespace CppUtils;

#ifdef __APPLE__
    #include <libgen.h>
    #include <limits.h>
    #include <mach-o/dyld.h>
    #include <unistd.h>
#endif


static int TEXTWIDTH = 100;
static std::string TITLE = "Sublime Project Generator";

enum PType {
	Basic = 0, //		just cpp and include folders
	Standalone, // 		application folder, library inclusion ability
	Library, //			library folder, library inclusion ability
	AppEngine, //		application folder (uses library) and library folder (with library inclusion)
	
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


// Private function definitions
static bool OpenFolder(const std::string& initialPath, std::filesystem::path& fileResult);
static void SpicyCopy(const Project& prj, const std::filesystem::path& fromFolder, const std::filesystem::path& toFolder);
static void SearchReplace(std::string& str, std::string searchTerms[], std::string replaceTerms[], int numTerms);

static void PrintTitle();
static void PrintProject(Project prj, int param = 0);
static inline bool IsNotAlnumSpace(const char& c);
static void WrapText(std::string& text, int indentL = 4, int indentR = 1);

static PType StrToPType(const std::string& str);
static std::string PTypeToStr(const PType& type, const bool isFolder = false);

static void FixFolderNames(Project& prj);

static void ImportProjectInfo(Project& prj, const std::filesystem::path sublPrjFile);
static std::string ExportProjectInfo(const Project& prj);


int main(int argc, char* argv[]) {
	
	system("tabs -4");
	CppUtils_Init();
	
	
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
				switch (arg[1]) {
					
					case 'b':
						p.type = PType::Basic;
						break;
					case 's':
						p.type = PType::Standalone;
						break;
					case 'l':
						p.type = PType::Library;
						break;
					case 'a':
						p.type = PType::AppEngine;
						break;
					default:
						p.type = PType::Basic;
				}
			}
			else {
				
				// Name argument
				p.name = arg;
			}
		}
	}
	else {
		
		// Prompt user for new project name
		while (find_if(inputText.begin(), inputText.end(), IsNotAlnumSpace) != inputText.end()) {
			system("clear");
			PrintTitle();
			std::cout << std::left << std::setw(TEXTWIDTH / 5 - 2) << "\tProject name: ";
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
		
		ImportProjectInfo(p, sublPrjFile);
		
		if (p.isGenerated) {
			
			oldP = p;
		}
	}
	else {
		
		// set generic (default) values
		FixFolderNames(p);
	}
	
	
	while (true) {
		
		
		if (!editingParam) {
			// ! Main prompt and information
			system("clear");
			PrintProject(p); // print all project information
			
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
		PrintProject(p, currentParam);
		
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
		
		PType t = StrToPType(inputText);
		std::filesystem::path res;
		switch (currentParam) {
			
			case 1: // name -> changes all folder names & resets executable
				if (find_if(inputText.begin(), inputText.end(), IsNotAlnumSpace) == inputText.end()) {
					
					// as long as it is alphanumeric, allow it
					p.name = inputText;
					
					FixFolderNames(p);
					
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case 2: // type -> changes all folder names & resets executable
				if (t != PType::None) {
					p.type = t;
					
					FixFolderNames(p);
					
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not one of the four types";
				}
				break;
				
			case 3: // executable name
				if (find_if(inputText.begin(), inputText.end(), IsNotAlnumSpace) == inputText.end()) {
					
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
					
					if (OpenFolder(p.directory.string(), res)) {
						
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
				if (find_if(inputText.begin(), inputText.end(), IsNotAlnumSpace) == inputText.end()) {
					
					// alphanumeric only
					p.projectFolder = inputText;
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case 6: // app folder
				if (find_if(inputText.begin(), inputText.end(), IsNotAlnumSpace) == inputText.end()) {
					
					// alphanumeric only
					p.appFolder = inputText;
					continue;
				}
				else {
					
					inputText = "'" + inputText + "' is not alphanumeric";
				}
				break;
				
			case 7: // lib folder
				if (find_if(inputText.begin(), inputText.end(), IsNotAlnumSpace) == inputText.end()) {
					
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
	assetsDir = assetsDir / PTypeToStr(p.type, true);
	File::createDirIfNotExists(projectDir);
	
	// spicy copy each file recursively from asset directory
	SpicyCopy(p, assetsDir, projectDir); 
	// todo: currently only generates projects, or generates over projects: implement editing (replacing terms) in projects
	// todo: also implement moving of projects via changing of project directory
	
	std::cout << "\n\tDone." << std::endl;
	
	return 0;
}



static bool OpenFolder(const std::string& initialPath, std::filesystem::path& fileResult) {
	
	nfdchar_t *outPath;
	nfdresult_t nfdResult = NFD_PickFolder(&outPath, initialPath.c_str());
	
	if (nfdResult == NFD_OKAY) {
		
		fileResult = outPath;
		// Logger::Info("Resulting open folder value: '%s'.", fileResult.string().c_str());
		NFD_FreePath(outPath);
		return true;
	}
	else if (nfdResult == NFD_CANCEL) {
		
		// user pressed cancel
		return false;
	}
	
	// Logger::Error("While opening folder: %s", NFD_GetError());
	return false;
}

static void SpicyCopy(const Project& prj, const std::filesystem::path& fromFolder, const std::filesystem::path& toFolder) {
	
	std::string appFolderCaps = prj.appFolder;
	std::transform(appFolderCaps.begin(), appFolderCaps.end(), appFolderCaps.begin(), ::toupper);
	std::string libFolderCaps = prj.libFolder;
	std::transform(libFolderCaps.begin(), libFolderCaps.end(), libFolderCaps.begin(), ::toupper);
	
	int numKeys = 7;
	std::string searchWords[] = {"sgPath", "sgName", "sgExe", "sgAppName", "SGAPPNAME", "sgLibName", "SGLIBNAME"};
	std::string replaceWords[] = {prj.directory / prj.projectFolder, prj.name, prj.exeName, prj.appFolder, appFolderCaps, prj.libFolder, libFolderCaps};
	
	// create all folders 
	for (std::filesystem::path& p : File::getFoldersInDir(fromFolder)) {
		
		std::string folderName = p.filename();
		
		// replace specified terms in folder name
		SearchReplace(folderName, searchWords, replaceWords, numKeys);
		
		File::createDirIfNotExists(toFolder / folderName);
		SpicyCopy(prj, p, toFolder / folderName);
	}
	
	// copy, then modify each file (to add project info)
	for (std::filesystem::path& p : File::getFilesInDir(fromFolder)) {
		
		std::string filename = p.stem();
		
		// skip unnecessary files
		if (filename == ".DS_Store") {
			continue;
		}
		
		// replace specified terms in filename if necessary
		SearchReplace(filename, searchWords, replaceWords, numKeys);
		
		// copy to new directory and edit at the same time
		std::filesystem::path newFile = toFolder / (filename + p.extension().string());
		
		// open file to copy and new (copy) file
		std::ifstream inputFile(p.string());
		std::ofstream outputFile(newFile.string());
		
		if (inputFile.is_open() && outputFile.is_open()) {
			
			std::string line;
			
			while (std::getline(inputFile, line)) {
				
				// replace specified terms in line if necessary
				SearchReplace(line, searchWords, replaceWords, numKeys);
				
				outputFile << line << "\n";
			}
			
			// add project information to .sublime-project file
			if (newFile.extension() == ".sublime-project") {
				
				outputFile << "\n" << ExportProjectInfo(prj);
			}
			
			inputFile.close();
			outputFile.close();
		}
	}
}

static void SearchReplace(std::string& str, std::string searchTerms[], std::string replaceTerms[], int numTerms) {
	
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

static void PrintTitle() {
	
	std::cout << std::right << "\n\n" << std::setw(int((TEXTWIDTH - TITLE.length()) / 2) + 1) << std::setfill('-') << "-" 
			  << TITLE  << std::setw(int((TEXTWIDTH - TITLE.length()) / 2) + 2) << "-\n\n" << std::setfill(' '); // Title
}

static void PrintProject(Project prj, int param) {
	
	// sizing and temp vars
	int widthL = TEXTWIDTH / 5 - 2;
	int widthR = TEXTWIDTH / 5 * 4 - 5;
	std::string tmp = "";
	std::string tmp2 = "";
	
	
	PrintTitle();
	
	std::cout << std::left;
	
	if (param == 0 || param == 1) {
		
		std::cout << std::setw(widthL) << "\tProject name:" << std::setw(widthR) << prj.name << " (1)\n";
		
		if (param == 1) {
			
			tmp = std::string("\tThe name of the project that will be generated by this program. ") +
					  "Specifically, this variable is used in the main CMakeLists.txt file as the project name, as well as in the .sublime-project file as both the file and project name. " + 
					  "By default, the project folder and app folder (when applicable) also use the project name. " +
					  "Warning: this WILL change the project, app, and library folder names (when applicable)." + 
					  "\n\n\tSpecify the project name from command line as a single string (eg. 'Taco').";
			WrapText(tmp);
			std::cout << "\n" << tmp << "\n";
		}
	}
	
	if (param == 0 || param == 2) {
		
		std::cout << std::setw(widthL) << "\tProject type:" << std::setw(widthR) << PTypeToStr(prj.type) << " (2)\n";
		
		if (param == 2) {
			
			tmp = std::string("\tThe type of project that will be generated by this program. ") +
					  "Basic projects involve only the cpp and include folders, and generate a simple executable. " + 
					  "Standalone projects are for app creation that may involve external libraries. " +
					  "Library is a project type that is meant for custom library creation, and may include external libraries. " +
					  "Application & Engine combines the Standalone and Library types into one, in which both an app and 'engine' (custom library) are built. " +
					  "Warning: this WILL change the project, app, and library folder names (when applicable). " +
					  "\n\n\tUse -b, -s, -l, and -a as command line arguments (setting to Basic, Standalone, Library, and Application & Engine respectively).";
			WrapText(tmp);
			std::cout << "\n" << tmp << "\n";
		}
	}
	
	if (param == 0) {
		std::cout << "\n";
	}
	
	if (param == 0 || param == 3) {
		
		std::cout << std::setw(widthL) << "\tExecutable name:" << std::setw(widthR) << prj.exeName << " (3)\n";
		
		if (param == 3) {
			
			tmp = std::string("\tThe name of the executable that will be generated by this project.");
			WrapText(tmp);
			std::cout << "\n" << tmp << "\n";
		}
	}
	
	if (param == 0 || param == 4) {
		
		// directory splitting
		tmp = prj.directory.string();
		if (tmp.size() >= widthR) {
			
			for (int i = widthR - 1; i > 0; i--) {
				
				if (tmp[i] == '/') {
					
					tmp2 = tmp.substr(i+1, tmp.length());
					tmp = tmp.substr(0, i+1) + "\n";
					break;
				}
			}
			
			std::cout << std::setw(widthL) << "\tGen directory:" << std::setw(widthR) << tmp 
					  << std::setw(widthL) << "\t " << std::right << std::setw(widthR-1) << tmp2 << "  (4)\n" << std::left;
		} else {
			
			std::cout << std::setw(widthL) << "\tGen directory:" << std::setw(widthR) << tmp << " (4)\n";
		}
		
		if (param == 4) {
			
			tmp = std::string("\tThe directory in which the project (folder) will be generated by this program. ") +
					  "Note that the project resources/files will be created within said project folder (not within the directory itself).";
			WrapText(tmp);
			std::cout << "\n" << tmp << "\n";
		}
	}
	
	if (param == 0) {
		std::cout << "\n";
	}
	
	if (param == 0 || param == 5) {
		
		tmp = "../" + prj.directory.filename().string() + "/" + prj.projectFolder;
		std::cout << std::setw(widthL) << "\tProject folder:" << std::setw(widthR) << tmp << " (5)\n";
		
		if (param == 5) {
			
			tmp = std::string("\tThe folder in which the project will be generated by this program (in Directory/ProjectFolder). ") +
					  "This is where the project resources/files will be created.";
			WrapText(tmp);
			std::cout << "\n" << tmp << "\n";
		}
	}
	
	if (param == 0 || param == 6) {
		
		// only display app folder if it exists
		tmp = prj.appFolder;
		tmp2 = " (x)\n";
		if (tmp != "N/A") {
			
			tmp = "./" + prj.projectFolder + "/" + tmp;
			tmp2 = " (6)\n";
		}
		std::cout << std::setw(widthL) << "\tApp folder:" << std::setw(widthR) << tmp << tmp2;
		
		if (param == 6) {
			
			tmp = std::string("\tThe folder in which either a standalone or combined application's resources will be generated (in Directory/ProjectFolder/AppFolder). ") +
					  "This includes 'assets', 'cpp', and 'include' folders, along with the CMakeLists.txt file necessary to generate the executable. " +
					  "Also includes 'libraries' folder when generating standalone application.";
			WrapText(tmp);
			std::cout << "\n" << tmp << "\n";
		}
	}
	
	if (param == 0 || param == 7) {
		
		// only display lib folder if it exists
		tmp = prj.libFolder;
		tmp2 = " (x)\n";
		if (tmp != "N/A") {
			
			tmp = "./" + prj.projectFolder + "/" + tmp;
			tmp2 = " (7)\n";
		}
		std::cout << std::setw(widthL) << "\tLibrary folder:" << std::setw(widthR) << tmp << tmp2;
		
		if (param == 7) {
			
			tmp = std::string("\tThe folder in which either a library or combined application's private library/engine resources will be generated (in Directory/ProjectFolder/LibFolder). ") +
					  "This includes 'cpp', 'include', and 'libraries' folders, along with the CMakeLists.txt file necessary to generate the library.";
			WrapText(tmp);
			std::cout << "\n" << tmp << "\n";
		}
	}
	
	if (param == 0) {
		
		// "process" key
		std::cout << std::setw(TEXTWIDTH - 6) << "\n\t-> " +
				std::string(prj.isGenerated ? "Edit" : "Generate") + " Project" << " (0)\n";
	}
	
	// separator (-----)
	std::cout << "\n" << std::setw(TEXTWIDTH) << std::setfill('-') << "-" << std::setfill(' ') << std::endl;
}

static inline bool IsNotAlnumSpace(const char& c) {
	
	// list of allowed characters (letters, numbers, and spaces)
	return !(isalpha(c) || isdigit(c) || (c == ' '));
}

static void WrapText(std::string& text, int indentL, int indentR) {
	
	// output text and streamed text
	std::string tmpText = "";
	tmpText.append(int(indentL / 4), '\t'); tmpText.append(indentL % 4, ' ');
	std::stringstream s(text);
	
	// tracking variables
	std::string word;
	std::string btwStr;
	int lastPos = 0; int pos;
	int lineLength = indentL;
	
	while (s >> word) {
		
		// Use previous word and current word start to determine characters between
		pos = text.find(word, lastPos);
		btwStr = text.substr(lastPos, pos - lastPos);
		
		// Include tabs and newlines in length considerations
		for (char c : btwStr) {
			
			switch (c) {
				case ' ':
					break;
				case '\t':
					lineLength += int(lineLength/4) * 4 + 4;
					tmpText = tmpText + '\t';
					break;
				case '\n':
					lineLength = indentL;
					tmpText = tmpText + '\n';
					tmpText.append(int(indentL / 4), '\t'); tmpText.append(indentL % 4, ' ');
					break;
			}
		}
		
		if (lineLength + word.length() > TEXTWIDTH - indentR) {
			
			// start new line
			lineLength = indentL;
			tmpText = tmpText + '\n';
			tmpText.append(int(indentL / 4), '\t'); tmpText.append(indentL % 4, ' ');
		}
		
		// Always add word length and space length to total line length
		lineLength = lineLength + word.length() + 1;
		tmpText = tmpText + word + " ";
		
		lastPos = pos + word.length();
	}
	
	text = tmpText;
}

static PType StrToPType(const std::string& str) {
	
	if (str == "Basic") {
		return PType::Basic;
	}
	if (str == "Standalone") {
		return PType::Standalone;
	}
	if (str == "Library") {
		return PType::Library;
	}
	if (str == "Application & Engine" || str == "AppEngine") {
		return PType::AppEngine;
	}
	
	return PType::None;
}

static std::string PTypeToStr(const PType& type, const bool isFolder) {
	
	switch (type) {
		
		case PType::Basic:
			return "Basic";
			break;
		case PType::Standalone:
			return "Standalone";
			break;
		case PType::Library:
			return "Library";
			break;
		case PType::AppEngine:
			if (isFolder) {
				return "AppEngine";
			}
			return "Application & Engine";
			break;
		default:
			return "";
	}
}

static void FixFolderNames(Project& prj) {
	
	prj.exeName = "N/A";
	prj.projectFolder = prj.name;
	prj.appFolder = "N/A";
	prj.libFolder = "N/A";
	
	if (prj.type == PType::Standalone || prj.type == PType::AppEngine) {
		prj.exeName = "executable";
		prj.appFolder = prj.name;
	}
	if (prj.type == PType::Library) {
		// want libraries to show up as the project name, and allow user to determine whether 
		//  to add "Lib" in library name
		prj.libFolder = prj.name; // + "Lib"; 
	}
	if (prj.type == PType::AppEngine) {
		prj.exeName = "executable";
		prj.libFolder = prj.name + "Engine";
	}
}

static void ImportProjectInfo(Project& prj, const std::filesystem::path sublPrjFile) {
	
	std::ifstream inputFile(sublPrjFile.string());
	
	if (inputFile.is_open()) {
		
		std::string line;
		size_t pos;
		
		// bypass all lines in main file
		int count = 1;
		std::getline(inputFile, line); // skip first line: "{"
		while (std::getline(inputFile, line) && count > 0) {
			
			pos = line.find("{");
			
			while (pos != std::string::npos) {
				
				count++;
				pos = line.find("{", pos + 1);
			}
			
			pos = line.find("}");
			
			while (pos != std::string::npos) {
				
				count--;
				pos = line.find("}", pos + 1);
			}
		}
		
		
		// search for key terms in text
		while (std::getline(inputFile, line)) {
			
			if (!prj.isGenerated) {
				pos = line.find("Project Name");
				
				if (pos != std::string::npos) {
					
					prj.isGenerated = true;
				}
			}
			
			if (prj.isGenerated) {
				
				pos = line.find("'");
				
				if (pos != std::string::npos) {
					
					std::string val = line.substr(pos + 1, line.find("'", pos + 1) - pos - 1);
					std::cout << val << std::endl;
					
					switch (count) {
						case 0:
							prj.name = val;
							break;
						case 1:
							prj.type = StrToPType(val);
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
		if (!prj.isGenerated) {
			
			// set generic (default) values
			FixFolderNames(prj);
		}
		
		inputFile.close();
	}
	else {
		
		// set generic (default) values
		FixFolderNames(prj);
	}
}

static std::string ExportProjectInfo(const Project& prj) {
	
	std::string exportText;
	
	exportText = exportText + "// \n" +
							  "// SublimeGenerator (sg) Project Info\n" +
							  "// \n" +
							  "//  -> Run 'sg ProjectName' to modify these values:\n" +
							  "//   modification of string values that are equal may produce undesireable results.\n" +
							  "// \n" +
							  "// Project Name: '" + prj.name + "'\n" + 
							  "// Type: '" + PTypeToStr(prj.type) + "'\n" + 
							  "// Executable: '" + prj.exeName + "'\n" + 
							  "// Directory: '" + prj.directory.string() + "'\n" + 
							  "// Project Folder: '" + prj.projectFolder + "'\n" + 
							  "// App Folder: '" + prj.appFolder + "'\n" + 
							  "// Lib Folder: '" + prj.libFolder + "'\n" +
							  "// ";
	
	return exportText;
}