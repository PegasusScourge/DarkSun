/**

File: AudionEngine.cpp
Description:

Audio control

*/

#include "AudioEngine.hpp"

using namespace darksun;

// Init static vars
AudioEngine::SoundPlayer AudioEngine::soundPlayers[MAX_SOUND_PLAYERS];
std::map<string, AudioEngine::SoundBuf> AudioEngine::loadedBuffers = std::map<string, AudioEngine::SoundBuf>();
std::vector<AudioEngine::PlayRequest> AudioEngine::soundsToPlay = std::vector<AudioEngine::PlayRequest>();
std::vector<string> AudioEngine::soundsToStop = std::vector<string>();

void AudioEngine::init() {

	dout.log("---------- Audio engine init ----------");

	sf::Listener::setGlobalVolume(50.0f);
	sf::Listener::setPosition(0, 0, 0);
	sf::Listener::setUpVector(0, 1, 0);
	sf::Listener::setDirection(0, 0, 1);


	dout.log("Audio engine init complete");
}

void AudioEngine::addSound(string source) {
	// Check for duplicates
	if (loadedBuffers.count(source) > 0) {
		dout.warn("Attempted to load sound '" + source + "' but it already exists in memory, skipping");
		return;
	}

	SoundBuf buf;
	if (buf.loadBuf(source)) {
		// Success loading
		loadedBuffers[source] = buf;
	}
	else {
		// Failed to load
		dout.error("Failed to load sound '" + source + "'");
	}
}

void AudioEngine::removeSound(string ref) {
	if (loadedBuffers.count(ref) > 0) {
		loadedBuffers.erase(ref);
	}
	else {
		dout.warn("Attempted to remove sound '" + ref + "' but it doesn't exist in memory");
	}
}

void AudioEngine::playSound(string ref, bool loop, int startIndex) {
	PlayRequest request;
	request.ref = ref;
	request.looped = loop;
	request.startIndex = startIndex;

	soundsToPlay.push_back(request);
}

void AudioEngine::stopSound(string ref) {
	soundsToStop.push_back(ref);
}

void AudioEngine::tick(float deltaTime) {
	// Stop sounds
	for (const auto& s : soundsToStop) {
		for (int i = 0; i < MAX_SOUND_PLAYERS; i++) {
			if (soundPlayers[i].attachedRef.compare(s) == 0) {
				// Found our ref
				soundPlayers[i].stop();
				break;
			}
		}
	}
	soundsToStop.clear();

	// Play sounds
	for (const auto& s : soundsToPlay) {
		// Check for the ref existing
		if (loadedBuffers.count(s.ref) == 0) {
			// Doesn't exist
			dout.error("Tried to play sound '" + s.ref + "' but it isn't loaded!");
			continue; // Next request
		}

		for (int i = 0; i < MAX_SOUND_PLAYERS; i++) {
			if (soundPlayers[i].isStopped() || !soundPlayers[i].isAttached) {
				// We have a free player, attach and play here
				soundPlayers[i].isAttached = true;
				soundPlayers[i].attachedRef = s.ref;
				soundPlayers[i].sound.setBuffer(loadedBuffers[s.ref].buffer);
				
				// Set any other properties
				soundPlayers[i].sound.setLoop(s.looped);
				soundPlayers[i].sound.setPlayingOffset(sf::milliseconds(s.startIndex));
				soundPlayers[i].sound.setAttenuation(0.0f); // NO ATTENUATION
				soundPlayers[i].sound.setVolume(100.0f);

				// Set the sound going
				soundPlayers[i].play();

				dout.verbose("AudioEngine --> playing sound in bay " + std::to_string(i) + " from source '" + s.ref + "'");
				break;
			}
		}
	}
	soundsToPlay.clear();
}