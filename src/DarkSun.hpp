#pragma once
/**

File: DarkSun.hpp
Description:

Main app

*/

#include <filesystem>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

#include <chrono>

#include "ApplicationSettings.hpp"

#include "Renderer.hpp"
#include "Log.hpp"

#include "DarkSunProfiler.hpp"

#include "Scene.hpp"

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace darksun;

namespace darksun {

	class DarkSun {

	private:

		int OpenGLThread(std::shared_ptr<Renderer> renderer, ApplicationSettings* appSettings);

		std::atomic<bool> renderThreadStarted = false;
		std::atomic<bool> running = false;
		std::atomic<bool> hasFocus = false;
		std::atomic<bool> captureMouse = false;

		std::atomic<float> deltaTime_main = 0;
		std::atomic<float> deltaTime_render = 0;

		std::mutex activeScene_mutex;
		std::unique_ptr<Scene> activeScene = NULL;

	public:
		/* Default constructor */
		DarkSun();

		/* Processes the arguments from the command line */
		void processArgs(int argc, char *argv[]);

		/* Does the execution */
		void run();

	};

}