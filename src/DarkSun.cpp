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
	renderer.getCamera()->Position = glm::vec3(-15.0f, 20.0f, -5.0f);

	// Put a light under the spider at 0,0,0
	renderer.setLightPosition(1, glm::vec3(0, 5.5f, 0)); // Make sure it is above the floor
	renderer.setLightColor(1, glm::vec3(0, 0, 0));
	renderer.setLightAttenuation(1, true);

	// Test the models
	Shader defaultShader;
	// Start with gamma correction set off
	renderer.setGammaCorrection(defaultShader, false);

	Entity testEntity("test");
	testEntity.setPosition(glm::vec3(0.0f, 6.0f, 0.0f));
	Entity floorEntity("floor");

	dout.log("test isValid = " + std::to_string(testEntity.isValid()) + ", floor isValid = " + std::to_string(floorEntity.isValid()));

	sf::RenderWindow * window = renderer.getWindowHandle();

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

		// Clear the screen to black
		renderer.clearscreen();

		Camera *camera = renderer.getCamera();

		defaultShader.use();

		// Put in the lighting info

		renderer.prepLights(defaultShader);

		// view/projection matricies input

		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderer.SCREEN_WIDTH / (float)renderer.SCREEN_HEIGHT, 0.1f, 100.0f);
		//glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 view = glm::lookAt(camera->Position, testEntity.getPosition(), camera->WorldUp);
		defaultShader.setMat4("projection", projection);
		defaultShader.setMat4("view", view);
		
		//dout.log("Rendering");

		if (floorEntity.isValid()) {
			floorEntity.draw(defaultShader);
		}
		if (testEntity.isValid()) {
			testEntity.draw(defaultShader);
		}

		// Do the displaying
		window->display();

		// tick the engine
		//engine.tick();

		// Move the light in a circle (for now)
		renderer.setLightPosition(0, glm::vec3(10.0f * sinf(sinArg), 10.0f, 10.0f * cosf(sinArg)));
		renderer.setLightColor(1, glm::vec3(sinf(sinArg), 0.0f, cosf(sinArg)));
		sinArg += 0.05;

		//dout.log("Ending tick '" + to_string(tickNo) + "'");
		tickNo++;
	}

	renderer.cleanup();
}