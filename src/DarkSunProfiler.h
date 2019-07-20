#pragma once
/**

File: DarkSunProfiler.h
Description:

Uses black magic to monitor the functions that are being used and for how long

*/

#ifndef ENABLE_DS_PROFILING
	// Comment this define in and out to enable/disable profiling
	#define ENABLE_DS_PROFILING
#endif

#include <SFML/System.hpp>
#include <map>
#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>

using string = std::string;

namespace darksun::profiler {

	/**

	Starts a clock when constructed, and then the deconstructor automatically gets the elapsed time and posts to the profiling system
	when the profiler goes out of scope.
	
	*/
	struct ScopeProfiler {
		// Constructor
		ScopeProfiler(string ref);

		// Destructor
		~ScopeProfiler();

		// Vars
		string _ref;
		sf::Clock myClock;

	};

	/**
	
	Stores a map of the references and the time spent in the reference with an id

	*/
	struct ProfileFrame {
		std::map<string, int> times;
		int totalTime = 0;

		int frameId = 0;
	};

	void addToCurrentFrame(string ref, int millis);

	void newFrame();

	void dumpFrames(string file);

}