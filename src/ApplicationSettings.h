#pragma once
/**

File: ApplicationSettings.h
Description:

Stores settings for the application in a central place

Thread safed

*/

#include "LuaEngine.h"
#include <atomic>

using string = std::string;

namespace darksun {

	class ApplicationSettings {

	public:
		ApplicationSettings(string settingsFile);

		std::atomic<float> opengl_nearZ = 0.1f;
		std::atomic<float> opengl_farZ = 2000.0f;

		bool get_opengl_vsync() {
			return opengl_vsync.load();
		}
		void set_opengl_vsync(bool v) {
			opengl_vsync = v;
		}
		int get_opengl_depthBits() {
			return opengl_depthBits.load();
		}
		int get_opengl_stencilBits() {
			return opengl_stencilBits.load();
		}
		int get_opengl_antialiasingLevel() {
			return opengl_antialiasingLevel.load();
		}
		int get_opengl_majorVersion() {
			return opengl_majorVersion.load();
		}
		int get_opengl_minorVersion() {
			return opengl_minorVersion.load();
		}
		int get_opengl_framerateLimit() {
			return opengl_framerateLimit.load();
		}
		void set_opengl_framerateLimit(int v) {
			opengl_framerateLimit = v;
		}

	private:

		std::atomic<int> opengl_depthBits;
		std::atomic<int> opengl_stencilBits;
		std::atomic<int> opengl_antialiasingLevel;
		std::atomic<int> opengl_majorVersion;
		std::atomic<int> opengl_minorVersion;
		std::atomic<bool> opengl_vsync = false;
		std::atomic<int> opengl_framerateLimit = 200;

		LuaEngine engine;

		void loadSettings(string file);
	};

}
