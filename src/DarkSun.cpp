/**

File: DarkSun.cpp
Description:

Main app

*/

#include "DarkSun.hpp"

using namespace darksun;

DarkSun::DarkSun() {
	// Default constructor
}

void DarkSun::processArgs(int argc, char *argv[]) {
	// Do something here?
}

void DarkSun::run() {
	dout.log("DarkSun init");

	// Start the profiling
	profiler::writeProfilingHeader();

	running = true;

	// Load settings
	ApplicationSettings appSettings("settings.lua");

	// Init the audio engine
	AudioEngine::init();

	std::shared_ptr<Renderer> renderer = std::shared_ptr<Renderer>(new Renderer());
	dout.verbose("Renderer pointer created");

	// CREATE THE RENDERING THREAD
	//std::thread renderingThread(&DarkSun::OpenGLThread, this, renderer, &appSettings);
	std::future renderingThread = std::async(std::launch::async, &DarkSun::OpenGLThread, this, renderer, &appSettings);
	dout.log("Rendering thread created, waiting for launch...");

	while (!renderThreadStarted) {
		// Spin our wheels
	}
	dout.log("Detected start of rendering thread");

	// we use this info for recreating scenes too, nice way of passing information
	SceneInformation sceneInfo;
	sceneInfo.n = "testScene";
	sceneInfo.id = Scene::createNewId();
	sceneInfo.hasMap = true;

	{
		std::lock_guard lock(activeScene_mutex);
		activeScene = std::unique_ptr<Scene>(new Scene(renderer, &appSettings, sceneInfo));
		if (!activeScene->isValid()) {
			dout.error("SCENE IS NOT VALID!");
		}
	}

	dout.log("Entering the main game engine loop");

	hasFocus = false;
	captureMouse = false;

	int tickNo = 0;
	float sinArg = 0.0f;

	/* TEST AUDIO */
	//AudioEngine::playSound("sounds/LCday_3_mono.ogg", "default", true);

	sf::Clock clock; // starts the clock
	sf::Time elapsedTime = clock.getElapsedTime();

	while (running) {
		profiler::newFrame();
		profiler::ScopeProfiler myProfiler("DarkSun.cpp::DarkSun::run()");
		
		// We pretend as if time isn't moving forward here, and is only at the instance we take this clock reading
		elapsedTime = clock.getElapsedTime();
		deltaTime_main = elapsedTime.asSeconds();
		clock.restart();

		{
			std::lock_guard lock(activeScene_mutex);
			// tick the scene
			activeScene->tick(deltaTime_main);
		}

		// tick the audio engine
		AudioEngine::tick(deltaTime_main);

		{
			std::lock_guard lock(activeScene_mutex);
			
			// Pass events to the scene
			for (auto& e : mtopengl::getEvents()) {
				activeScene->handleEvent(e);
			}

			// Check for scene transitions
			if (activeScene->shouldTransition()) {
				string target = activeScene->getNewScene();

				if (target.compare("exit") == 0) {
					// Signal an exit
					dout.log("Scene gave order to exit with transition");
					running = false;
				}
				else {
					// Assign the new scene
					activeScene->close(); // Close old scene
					sceneInfo.n = target;
					sceneInfo.id = Scene::createNewId();
					sceneInfo.hasMap = true;
					activeScene = std::unique_ptr<Scene>(new Scene(renderer, &appSettings, sceneInfo));
					//activeScene->init();
					if (!activeScene->isValid()) {
						running = false;
						dout.error("TRIED TO SWITCH TO NEW SCENE '" + target + "' BUT SCENE WAS INVALID");
					}
					//activeScene->initTest();
				}
			}
		}

		tickNo++;

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(4ms);
	}

	dout.log("Waiting for rendering thread to close...");
	int returnVal = renderingThread.get();
	dout.log("Rendering thread closed, return val of " + std::to_string(returnVal));

	activeScene->close();
}

int DarkSun::OpenGLThread(std::shared_ptr<Renderer> renderer, ApplicationSettings* appSettings) {

	renderer->create(appSettings);
	dout.log("OpenGLThread() --> Rendering thread initialised");

	sf::RenderWindow * window = renderer->getWindowHandle();
	window->setVerticalSyncEnabled(appSettings->get_opengl_vsync());
	window->setFramerateLimit(appSettings->get_opengl_framerateLimit());
	dout.log("OpenGLThread() --> Access to window established");

	renderThreadStarted = true;

	// Wait for the active scene to be initialised
	bool cont = false;
	dout.log("OpenGLThread() --> Waiting for first scene to be created");
	while (!cont) {
		std::lock_guard lock(activeScene_mutex);
		if (activeScene != NULL)
			cont = true;
	}
	dout.log("OpenGLThread() --> Detected creation of scene");

	sf::Clock clock; // starts the clock
	sf::Time elapsedTime = clock.getElapsedTime();

	dout.verbose("OpenGLThread() --> Entering rendering thread loop");

	bool isCamEnabled = false;

	while (running) {
		profiler::ScopeProfiler myProfiler("DarkSun.cpp::OpenGLThread()");
		
		// We pretend as if time isn't moving forward here, and is only at the instance we take this clock reading
		elapsedTime = clock.getElapsedTime();
		deltaTime_render = elapsedTime.asSeconds();
		clock.restart();

		// Process opengl requests from other threads
		mtopengl::process();

		// Draw the scene
		renderer->render();

		// Finish drawing
		// Do the displaying
		renderer->getWindowHandle()->display();

		sf::Event event;
		while (window->pollEvent(event)) {
			profiler::ScopeProfiler eventPollingProfiler("DarkSun.cpp::DarkSun::OpenGLThread()eventPolling");

			// Check for window focus
			hasFocus = window->hasFocus();

			// Check for close orders
			if (event.type == sf::Event::Closed) {
				running = false;
				dout.log("Window gave order to exit with X");
			}

			// Only process these events if we have focus
			if (hasFocus) {
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::Escape:
						running = false;
						break;
					}
				}

				{
					// Don't block the locking attempt on the active scene here, to prevent deadlocks
					std::unique_lock lock(activeScene_mutex, std::try_to_lock);
					if(lock.owns_lock())
						isCamEnabled = activeScene->isCameraEnabled();
				}

				if (isCamEnabled) { // Only allow the camera to recieve input if the scene allows it
					renderer->getCamera()->handleEvent(event, deltaTime_render);
				}
			}

			// Set the mouse grabbing
			window->setMouseCursorGrabbed(hasFocus && captureMouse);
			window->setMouseCursorVisible(!(hasFocus && captureMouse));

			// Pass the event to the mtopengl solution
			mtopengl::addEvent(event);
		}
		// Poll the keyboard checks for the mouse
		if (hasFocus && activeScene->isCameraEnabled())
			renderer->getCamera()->pollKeyboard(deltaTime_render);

		// Update the listener from the AudioEngine
		renderer->getCamera()->updateCameraVectors();
		AudioEngine::update(renderer->getCamera()->getPosition(), glm::vec3(0, 1, 0), renderer->getCamera()->getFrontVector());

		// Update any settings we need to
		window->setVerticalSyncEnabled(appSettings->get_opengl_vsync());
		window->setFramerateLimit(appSettings->get_opengl_framerateLimit());

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2ms);
	}

	dout.log("OpenGLThread() --> Rendering thread exiting...");

	return 0;
}