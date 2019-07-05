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

	// Test the models
	Shader defaultShader;
	Model testModel("spider.obj");

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
		renderer.clearscreen();
		defaultShader.use();

		// view/projection matricies input
		Camera *camera = renderer.getCamera();
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderer.SCREEN_WIDTH / (float)renderer.SCREEN_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera->GetViewMatrix();
		defaultShader.setMat4("projection", projection);
		defaultShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -20.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
		defaultShader.setMat4("model", model);

		testModel.draw(defaultShader);

		// Do the displaying
		window->display();

		// tick the engine
		//engine.tick();
	}

	renderer.cleanup();
}