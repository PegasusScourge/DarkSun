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

	sf::RenderWindow * window = renderer.getWindowHandle();

	dout.log("Entering the main game engine loop");

	bool running = true;
	while (running) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				running = false;
		}

		window->clear();
		window->draw(shape);
		window->display();

		// tick the engine
		engine.tick();
	}

	renderer.cleanup();
}