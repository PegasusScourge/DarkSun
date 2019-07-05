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
	// Init our rendering
	Renderer renderer;
	renderer.create();

	// Test shapes
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	// Test the lua
	LuaEngine engine;
	// Add tick function
	engine.addFile("null.lua");

	// Test the model


	sf::RenderWindow * window = renderer.getWindowHandle();

	dout.log("Entering the main game engine loop");

	bool running = true;
	while (running) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				running = false;
		}

		//window->clear();
		//window->draw(shape);

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// Do the displaying
		window->display();

		// tick the engine
		engine.tick();
	}

	renderer.cleanup();
}