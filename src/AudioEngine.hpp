#pragma once

/**

File: AudionEngine.hpp
Description:

Audio control

*/

#include <SFML/Audio.hpp>
#include <map>

#include "Log.hpp"

#define MAX_SOUND_PLAYERS 128

using string = std::string;

namespace darksun {

	// Provides an audio engine interface
	class AudioEngine {

	private:
		/* private structs */
		struct SoundPlayer {
			sf::Sound sound;
			bool isAttached = false;
			string attachedRef = "";

			void play() {
				if (!isAttached) {
					dout.error("Attempted to play SoundObject that has no attached sound");
					return;
				}
				sound.play();
			}
			void stop() {
				if (!isAttached) {
					dout.error("Attempted to stop SoundObject that has no attached sound");
					return;
				}
				sound.stop();
			}
			bool isStopped() {
				if (sound.getStatus() == sf::SoundSource::Status::Stopped)
					return true;
				return false;
			}
		};

		struct SoundBuf {
			sf::SoundBuffer buffer;
			string ref = ""; // Reference is the file it is loaded from aka the path
			bool isLoaded = false;

			bool loadBuf(string src) {
				ref = src;
				isLoaded = buffer.loadFromFile(src);

				if (!isLoaded) {
					dout.error("Attempted to load sound '" + src + "' but got an internal error");
				}

				return isLoaded;
			}
		};

		struct PlayRequest {
			string ref;
			bool looped;
			int startIndex;
		};

		/* Private variables */
		static SoundPlayer soundPlayers[MAX_SOUND_PLAYERS];
		static std::map<string, SoundBuf> loadedBuffers;

		static std::vector<PlayRequest> soundsToPlay;
		static std::vector<string> soundsToStop;

	public:
		// Init the engine
		static void init();

		// Tick the engine
		static void tick(float deltaTime);

		// Register a sound
		static void addSound(string source);

		// Unregister a sound
		static void removeSound(string ref);

		// Play the sound
		static void playSound(string ref, bool loop = false, int startIndex = 0);

		// Stop the sound
		static void stopSound(string ref);
	};

}