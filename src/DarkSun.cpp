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

	std::shared_ptr<Renderer> renderer = std::shared_ptr<Renderer>(new Renderer());
	renderer->create();

	Scene activeScene(renderer, "testScene", Scene::createNewId());
	activeScene.init();
	activeScene.initTest();

	if (!activeScene.isValid()) {
		dout.error("SCENE IS NOT VALID!");
	}

	sf::RenderWindow * window = renderer->getWindowHandle();
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(2000);

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
		//dout.log("Starting tick '" + to_string(tickNo) + "'");
		
		// We pretend as if time isn't moving forward here, and is only at the instance we take this clock reading
		elapsedTime = clock.getElapsedTime();
		currentElapsed = elapsedTime.asSeconds();
		deltaTime = currentElapsed - lastElapsed;

		sf::Event event;
		while (window->pollEvent(event)) {
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
				if (activeScene.isCameraEnabled()) { // Only allow the camera to recieve input if the scene allows it
					renderer->getCamera()->handleEvent(event, deltaTime);
				}
			}

			// Set the mouse grabbing
			window->setMouseCursorGrabbed(hasFocus && captureMouse);
			window->setMouseCursorVisible(!(hasFocus && captureMouse));

			// Pass the event to the scene
			activeScene.handleEvent(event);
		}
		// Poll the keyboard checks for the mouse
		if(hasFocus && activeScene.isCameraEnabled())
			renderer->getCamera()->pollKeyboard(deltaTime);

		// Draw the scene entities
		activeScene.draw(activeScene.getDefaultShader());
		// Draw the scene UI, area to optimise
		window->pushGLStates();
		activeScene.drawUI();
		window->popGLStates();

		activeScene.tick(deltaTime);

		// Check for scene transitions
		if (activeScene.shouldTransition()) {
			string target = activeScene.getNewScene();

			if (target.compare("exit") == 0) {
				// Signal an exit
				running = false;
			}
			else {
				// Assign the new scene, framework doesn't exist yet
			}
		}

		// Do the displaying
		window->display();

		tickNo++;
		lastElapsed = currentElapsed;
	}

	activeScene.close();
}