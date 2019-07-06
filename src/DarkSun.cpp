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

	dout.log("Entering the main game engine loop");

	bool running = true;
	bool hasFocus = false;

	int tickNo = 0;
	float sinArg = 0.0f;

	sf::Clock clock; // starts the clock
	sf::Time elapsedTime = clock.getElapsedTime();
	float lastElapsedMS = 0;
	float currentElapsedMS = 0;
	float deltaTime = 0;

	while (running) {
		//dout.log("Starting tick '" + to_string(tickNo) + "'");
		
		// We pretend as if time isn't moving forward here, and is only at the instance we take this clock reading
		elapsedTime = clock.getElapsedTime();
		currentElapsedMS = elapsedTime.asMilliseconds();
		deltaTime = currentElapsedMS - lastElapsedMS;

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
					renderer->getCamera()->handleEvent(event, deltaTime / 1000.0f);
				}
			}

			// Set the mouse grabbing
			window->setMouseCursorGrabbed(hasFocus);
			window->setMouseCursorVisible(!hasFocus);

			// Pass the event to the scene
			activeScene.handleEvent(event);
		}
		// Poll the keyboard checks for the mouse
		if(hasFocus && activeScene.isCameraEnabled())
			renderer->getCamera()->pollKeyboard(deltaTime/1000.0f);

		activeScene.draw(activeScene.getDefaultShader());
		activeScene.tick();

		// Do the displaying
		window->display();

		tickNo++;
		lastElapsedMS = currentElapsedMS;
	}

	activeScene.close();
}