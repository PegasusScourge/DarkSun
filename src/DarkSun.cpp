/**

File: DarkSun.cpp
Description:

Main app

*/

#include "DarkSun.h"

using namespace darksun;

DarkSun::DarkSun() {
	// Default constructor
}

void DarkSun::processArgs(int argc, char *argv[]) {
	// Do something here?
}

void DarkSun::run() {
	dout.log("DarkSun init");

	ApplicationSettings appSettings;

	std::shared_ptr<Renderer> renderer = std::shared_ptr<Renderer>(new Renderer());
	renderer->create(appSettings);

	// we use this info for recreating scenes too, nice way of passing information
	SceneInformation sceneInfo;
	sceneInfo.n = "testScene";
	sceneInfo.id = Scene::createNewId();
	sceneInfo.hasMap = true;

	std::unique_ptr<Scene> activeScene = std::unique_ptr<Scene>(new Scene(renderer, appSettings, sceneInfo));
	//activeScene->init();
	//activeScene->initTest();

	if (!activeScene->isValid()) {
		dout.error("SCENE IS NOT VALID!");
	}

	sf::RenderWindow * window = renderer->getWindowHandle();
	window->setVerticalSyncEnabled(appSettings.opengl_vsync);
	window->setFramerateLimit(appSettings.opengl_framerateLimit);

	dout.log("Entering the main game engine loop");

	bool running = true;
	bool hasFocus = false;
	bool captureMouse = false;

	int tickNo = 0;
	float sinArg = 0.0f;

	sf::Clock clock; // starts the clock
	sf::Time elapsedTime = clock.getElapsedTime();
	float lastElapsed = 0;
	float currentElapsed = 0;
	float deltaTime = 0;

	while (running) {
		profiler::newFrame();
		//profiler::ScopeProfiler myProfiler("DarkSun.cpp::DarkSun::run()whileLoop");
		
		//dout.log("Starting tick '" + to_string(tickNo) + "'");
		
		// We pretend as if time isn't moving forward here, and is only at the instance we take this clock reading
		elapsedTime = clock.getElapsedTime();
		currentElapsed = elapsedTime.asSeconds();
		deltaTime = currentElapsed - lastElapsed;

		sf::Event event;
		while (window->pollEvent(event)) {
			profiler::ScopeProfiler eventPollingProfiler("DarkSun.cpp::DarkSun::run()eventPolling");
			
			// Check for window focus
			hasFocus = window->hasFocus();

			// Check for close orders
			if (event.type == sf::Event::Closed) {
				running = false;
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
				if (activeScene->isCameraEnabled()) { // Only allow the camera to recieve input if the scene allows it
					renderer->getCamera()->handleEvent(event, deltaTime);
				}
			}

			// Set the mouse grabbing
			window->setMouseCursorGrabbed(hasFocus && captureMouse);
			window->setMouseCursorVisible(!(hasFocus && captureMouse));

			// Pass the event to the scene
			activeScene->handleEvent(event);
		}
		// Poll the keyboard checks for the mouse
		if(hasFocus && activeScene->isCameraEnabled())
			renderer->getCamera()->pollKeyboard(deltaTime);

		// Draw the scene
		renderer->render();

		// Finish drawing
		// Do the displaying
		window->display();

		// tick the scene
		activeScene->tick(deltaTime);

		// Check for scene transitions
		if (activeScene->shouldTransition()) {
			string target = activeScene->getNewScene();

			if (target.compare("exit") == 0) {
				// Signal an exit
				running = false;
			}
			else {
				// Assign the new scene
				activeScene->close(); // Close old scene
				sceneInfo.n = target;
				sceneInfo.id = Scene::createNewId();
				sceneInfo.hasMap = true;
				activeScene = std::unique_ptr<Scene>(new Scene(renderer, appSettings, sceneInfo));
				//activeScene->init();
				if (!activeScene->isValid()) {
					running = false;
					dout.error("TRIED TO SWITCH TO NEW SCENE '" + target + "' BUT SCENE WAS INVALID");
				}
				//activeScene->initTest();
			}
		}

		// Update any settings we need to
		window->setVerticalSyncEnabled(appSettings.opengl_vsync);
		window->setFramerateLimit(appSettings.opengl_framerateLimit);

		tickNo++;
		lastElapsed = currentElapsed;
	}

	activeScene->close();

	// Output the profiling stuff
	dout.log("DUMPING PROFILING INFO, this may take a moment....");
	profiler::dumpFrames("DarkSun.profile");
}