#pragma once
/**

File: Log.h
Description:

Header file for Log.cpp, provides standard logging functionality for the program

*/

#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

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

	public:
		Log(std::string file) : outs(file), outf(file) {};
		~Log();

		void log(std::string v);
		void error(std::string v);
	};

	extern Log dout;
	extern Log dlua;

}