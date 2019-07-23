/**

File: DarkSunProfiler.h
Description:

Uses black magic to monitor the functions that are being used and for how long

*/

#include "DarkSunProfiler.h"

using namespace darksun;

static string output = "DarkSun.profile";

// Current profile frame being written to
profiler::ProfileFrame currentFrame;

int fId = 0;

sf::Clock frameTimer;

std::mutex profilingMutex;

profiler::ScopeProfiler::ScopeProfiler(string ref) {
#ifdef ENABLE_DS_PROFILING
	_ref = ref;
	myClock = sf::Clock(); // start the clock
#endif
}

profiler::ScopeProfiler::~ScopeProfiler() {
#ifdef ENABLE_DS_PROFILING
	sf::Time elapsed = myClock.getElapsedTime();
	// Add to the current frame
	addToCurrentFrame(_ref, elapsed.asMicroseconds());
#endif
}

void profiler::addToCurrentFrame(string ref, int millis) {
#ifdef ENABLE_DS_PROFILING
	std::lock_guard lock(profilingMutex);
	if (currentFrame.times.count(ref) > 0) {
		// We already have an entry, increment it
		currentFrame.times[ref] += millis;
	}
	else {
		// Create the entry
		currentFrame.times[ref] = millis;
	}
#endif
}

void profiler::newFrame() {
#ifdef ENABLE_DS_PROFILING
	std::lock_guard lock(profilingMutex);
	currentFrame.totalTime = frameTimer.getElapsedTime().asMicroseconds();
	frameTimer.restart();
	
	dumpFrame();

	currentFrame = ProfileFrame(); // Reset the current frame
	currentFrame.frameId = ++fId;
#endif
}

void profiler::dumpFrame() {
#ifdef ENABLE_DS_PROFILING
	//std::lock_guard lock(profilingMutex);
	
	// Only print detailed information for every 200th frame or if the total frame time is > 100ms
	if (currentFrame.frameId % 200 != 0 && currentFrame.totalTime / 1000.0f < 100) {
		return;
	}

	// The out stream
	std::ofstream outs(output, std::ios_base::app);
		
	outs << " # ----- [ FRAME " << std::to_string(currentFrame.frameId) << " ] ----- #" << std::endl;
	outs << "TOTAL FRAME TIME: " << std::to_string((float)currentFrame.totalTime / 1000.0f) << "ms" << std::endl;

	// Output the information for each reference
	for (auto const& time : currentFrame.times) {
		outs << " - Ref '" << time.first << "', " << std::to_string((float)time.second / 1000.0f) << "ms" << std::endl;
	}

	outs.flush();
	outs.close(); // Close the stream
#endif
}

void profiler::writeProfilingHeader() {
#ifdef ENABLE_DS_PROFILING
	std::lock_guard lock(profilingMutex);
	// The out stream
	std::ofstream outs(output);

	outs << "[ PROFILING STARTED ]" << std::endl << std::endl;

	outs.flush();
	outs.close();
#endif
}