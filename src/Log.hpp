#pragma once
/**

File: Log.hpp
Description:

Header file for Log.cpp, provides standard logging functionality for the program

*/

#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <mutex>

#define VERBOSE_LOGGING_DARKSUN true

namespace darksun {

	inline std::string BoolToString(bool b)
	{
		return b ? "true" : "false";
	}

	class Log {

	private:
		// Gives the current time as a timestamp string for logging purposes
		std::string timestamp();

		// The log stream
		std::ofstream outs;

		// The file we are writing to (for admin purposes)
		std::string outf;

		bool isVerbose = false;

		std::mutex myMutex;

	public:
		Log(std::string file, bool verbose = false) : outs(file), outf(file), isVerbose(verbose) {};
		~Log();

		void log(std::string v);
		void error(std::string v);
		void warn(std::string v);
		void verbose(std::string v);
	};

	extern Log dout;
	extern Log dlua;

}