// TextFormatter.h
// -> Functions and settings for text formatting, specifically intended for terminal logging
// Author: Justin Bunting
// Created: 2025/10/01
// Last Modified: 2025/10/01 11:36

#ifndef _TEXTFORMATTER_H
#define _TEXTFORMATTER_H


#include <cstdio>
#include <string>

#include "Project.h"

namespace TextFormatter {
	
	static int TEXT_WIDTH = 100;
	
	static void printTitle(std::string title);
	static void printProject(const Project& prj, std::string title, int param = 0);
	
	static inline bool isNotAlnumSpace(const char& c);
	static void wrapText(std::string& text, int textWidth, int indentL = 4, int indentR = 1);
}


#endif // _TEXTFORMATTER_H