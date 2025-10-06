// TextFormatter.h
// -> Functions and settings for text formatting, specifically intended for terminal logging
// Author: Justin Bunting
// Created: 2025/10/01
// Last Modified: 2025/10/05 15:10

#ifndef _TEXTFORMATTER_H
#define _TEXTFORMATTER_H


#include <cstdio>
#include <string>

#include "Project.h"

// used in main and TextFormatter: this is the best place to put it so both have access
enum EditParam {
	Build = 0,
	Name, // 			1
	Type, // 			2
	ExeName, // 		3
	Directory, //		4
	ProjectFolder, //	5
	AppFolder, //		6
	LibFolder //		7
};

namespace TextFormatter {
	
	static int TEXT_WIDTH = 100;
	static int WIDTH_L = TEXT_WIDTH / 5 - 1;
	static int WIDTH_R = TEXT_WIDTH / 5 * 4 - 6;
	
	void printTitle(std::string title);
	void printProject(const Project& prj, std::string title, int param = 0);
	void printProjectFolders(const Project &prj);
	void printFolderTree(std::filesystem::path &path, const Project &prj, const std::string &prefix, bool isLast = false);
	
	static inline bool isNotAlnumSpace(const char& c) {

        // list of allowed characters (letters, numbers, and spaces)
        return !(isalpha(c) || isdigit(c) || (c == ' '));
    }
	
	void wrapText(std::string& text, int textWidth, int indentL = 4, int indentR = 1);
	void wrapFileName(std::string& fileText, int textWidth, int indentAfterBreak = 4, int indentL = 4, int indentR = 1);

}


#endif // _TEXTFORMATTER_H