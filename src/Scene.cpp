/**

File: Scene.cpp
Description:

Defines all scenes

*/

#include "Scene.h"

using namespace darksun;

long Scene::LastSceneId = 0;

long Scene::createNewId() {
	// Return a new numerical Id
	return ++LastSceneId;
}

void Scene::init() {
	dout.log("Creating scene '" + sceneName + "'");

	// Start with gamma correction set off
	renderer->setGammaCorrection(defaultShader, false);

	dout.log("Scene creation complete");
	valid = true;
}

void Scene::initTest() {
	dout.log("initTest() called on scene '" + sceneName + "'");
	renderer->getCamera()->Position = glm::vec3(-15.0f, 28.0f, -10.0f);
	
	// Put a light under the spider at 0,0,0
	renderer->setLightPosition(1, glm::vec3(0, 5.5f, 0)); // Make sure it is above the floor
	renderer->setLightColor(1, glm::vec3(0, 0, 0));
	renderer->setLightAttenuation(1, true);

	dout.log(" - Camera and lighting setup as required");

	std::shared_ptr<Entity> testEntity = std::shared_ptr<Entity>(new Entity("test"));
	testEntity->setPosition(glm::vec3(0.0f, 6.0f, 0.0f));
	entities.push_back(testEntity);

	dout.log(" - Added test entity");

	std::shared_ptr<Entity> floorEntity = std::shared_ptr<Entity>(new Entity("floor"));
	entities.push_back(floorEntity);

	dout.log(" - Added floor entity");
	dout.log("test isValid = " + std::to_string(testEntity->isValid()) + ", floor isValid = " + std::to_string(floorEntity->isValid()));

	dout.log("initTest() complete");
}

void Scene::close() {

}

void Scene::draw(Shader* shader) {
	if (!valid) {
		dout.error("ATTEMPTED TO DRAW INVALID SCENE!");
		return;
	}

	// Clear the screen to black
	renderer->clearscreen();

	Camera *camera = renderer->getCamera();

	shader->use();

	// Put in the lighting info

	renderer->prepLights(shader);

	// view/projection matricies input

	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderer->SCREEN_WIDTH / (float)renderer->SCREEN_HEIGHT, 0.1f, 100.0f);
	//glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 view = glm::lookAt(camera->Position, glm::vec3(0,0,0), camera->WorldUp);
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	// Draw the entities
	for (auto &e : entities) {
		if (e->isValid()) {
			e->draw(shader);
		}
		else {
			dout.warn("Invalid entity detected in scene '" + sceneName + "', attempted to draw. Probably hasn't been collected for garbage yet (entityId = '" + std::to_string(e->getId()) + "')");
		}
	}

	// Draw the UI?
	// Well we have none so far
}

void Scene::tick() {
	// Move the light in a circle (for now)
	renderer->setLightPosition(0, glm::vec3(10.0f * sinf(sinArg), 10.0f, 10.0f * cosf(sinArg)));
	renderer->setLightColor(1, glm::vec3(sinf(sinArg), 0.0f, cosf(sinArg)));
	sinArg += 0.05;
}