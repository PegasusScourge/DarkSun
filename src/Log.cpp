/**

File: Log.cpp
Description:

Provides standard logging functionality for the program

*/
#include "Log.h"

using namespace darksun;

std::string Log::timestamp() {
	std::ostringstream stream;
	time_t rawtime;
	tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int hour = timeinfo->tm_hour;
	if (hour < 10)
		stream << "0";
	stream << hour << ":";

	int min = timeinfo->tm_min;
	if (min < 10)
		stream << "0";
	stream << min << ":";

	int sec = timeinfo->tm_sec;
	if (sec < 10)
		stream << "0";
	stream << sec;
	// The str() function of output stringstreams return a std::string.
	return stream.str();
}

Log::~Log() {
	outs.close();
}

void Log::log(std::string v) {
	std::cout << "[" << outf << "][" << timestamp() << "] " << v << std::endl;
	outs << "[" << timestamp() << "] " << v << std::endl;
}

void Log::error(std::string v) {
	log("[ERROR] " + v);
	if (this != &darksun::dout) // If we aren't the std log
		darksun::dout.log("ERROR logged in " + outf); // Notify the std log that there was an error we need to see
}

void Log::warn(std::string v) {
	log("[warn] " + v);
}

void Log::verbose(std::string v) {
	if(isVerbose)
		log("[verbose] " + v);
}

Log darksun::dout("DarkSun.log", true);
Log darksun::dlua("DarkSun_lua.log", true);
