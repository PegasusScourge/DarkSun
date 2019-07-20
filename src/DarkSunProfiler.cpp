/**

File: DarkSunProfiler.h
Description:

Uses black magic to monitor the functions that are being used and for how long

*/

#include "DarkSunProfiler.h"

using namespace darksun;

// Stores committed frames
std::vector<profiler::ProfileFrame> frames;

// Current profile frame being written to
profiler::ProfileFrame currentFrame;

int fId = 0;

sf::Clock frameTimer;

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
	currentFrame.totalTime = frameTimer.getElapsedTime().asMicroseconds();
	frameTimer.restart();
	frames.push_back(currentFrame);
	currentFrame = ProfileFrame(); // Reset the current frame
	currentFrame.frameId = ++fId;
#endif
}

void profiler::dumpFrames(string file) {
#ifdef ENABLE_DS_PROFILING
	// The out stream
	std::ofstream outs(file);

	for (auto& e : frames) {
		// Only print detailed information for every 20th frame or if the total frame time is > 100ms
		if (e.frameId % 20 != 0 && e.totalTime / 1000.0f < 100) {
			continue;
		}
		
		outs << " # ----- [ FRAME " << std::to_string(e.frameId) << " ] ----- #" << std::endl;
		outs << "TOTAL FRAME TIME: " << std::to_string((float)e.totalTime / 1000.0f) << "ms" << std::endl;

		int registeredTime = 0;

		// Output the information for each reference
		for (auto const& time : e.times) {
			outs << " - Ref '" << time.first << "', " << std::to_string((float)time.second / 1000.0f) << "ms" << std::endl;
			registeredTime += time.second;
		}

		int unknownTime = e.totalTime - registeredTime;
		outs << " - Unknown Ref, " << std::to_string((float)unknownTime / 1000.0f) << "ms" << std::endl;
	}

	outs.flush();
	outs.close(); // Close the stream
#endif
}