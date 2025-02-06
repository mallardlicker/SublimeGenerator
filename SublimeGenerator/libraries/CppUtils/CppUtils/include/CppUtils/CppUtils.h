#ifndef B_CPP_UTILS_H 
#define B_CPP_UTILS_H

#include "CppUtils/file/File.h"
#include "CppUtils/file/FileDialog.h"

#include <climits>
#include <functional>
#include <stdarg.h>
#include <cstdint>

#define NFD_NATIVE
#include <nfd.h>

namespace CppUtils {
	
	namespace TIME {
		#define SYS() TimeFunc()
		
		inline std::int64_t TimeFunc() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}
	}
	
	namespace Logger {

		#define Info(msg, ...) MacroWillCall(0, msg, 0, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
		#define Debug(msg, ...) MacroWillCall(0, msg, 1, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
		#define Warning(msg, ...) MacroWillCall(0, msg, 2, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
		#define Error(msg, ...) MacroWillCall(0, msg, 3, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
		#define Assert(val, msg, ...) MacroWillCall(val, msg, 4, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
		
		static std::string ToString(int severity) {
			switch(severity) {
				case 0:
					return "Info";
				case 1:
					return "Debug";
				case 2:
					return "Warning";
				case 3:
					return "Error";
				case 4:
					return "Assert";
				default:
					throw std::runtime_error("Severity not recognized.");
			}
		}

		inline void MacroWillCall(bool sendMsg, std::string msg, int severity, std::string fileName, int lineNum, std::string functionName, ...) {
			if (!sendMsg) {
				char* printfBuffer = new char[300];
				
				va_list args;
				va_start(args, functionName);
				vsnprintf(printfBuffer, 300, msg.c_str(), args);
				
				msg = std::string(printfBuffer);
				
				std::string formattedMsg;
				if (severity > 0) {
					formattedMsg += fileName + "::" + functionName + "(), line " + std::to_string(lineNum) + ".\n";
				}
				formattedMsg += "\t" + ToString(severity) + ": " + msg;
				
				std::cout << (formattedMsg) << std::endl;
				
				va_end (args);
				delete[] printfBuffer;
			}
		}
	} // Logger namespace
	
	inline void CppUtils_Init(bool doDebugText = false) {
		
		if (doDebugText) {
			
			Logger::Debug("Initializing CppUtils");
			
			Logger::Debug("Initializing NFD");
		}
		
		NFD_Init();
		
		if (doDebugText) {
			
			Logger::Debug("\t\tDone");
			
			Logger::Debug("Done initializing CppUtils");
		}
	}
}

#endif