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

	Scene testScene(renderer, "testScene");
	testScene.init();
	testScene.initTest();

	if (!testScene.isValid()) {
		dout.error("SCENE IS NOT VALID!");
	}

	sf::RenderWindow * window = renderer->getWindowHandle();

	dout.log("Entering the main game engine loop");

	bool running = true;
	int tickNo = 0;
	float sinArg = 0.0f;
	while (running) {
		//dout.log("Starting tick '" + to_string(tickNo) + "'");
		
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				running = false;
		}
		//dout.log("Rendering");

		testScene.draw(testScene.getDefaultShader());
		testScene.tick();

		// Do the displaying
		window->display();

		tickNo++;
	}

	testScene.close();
}