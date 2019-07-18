/**

File: Scene.cpp
Description:

Defines all scenes

*/

#include "Scene.h"

using namespace darksun;

int Scene::LastSceneId = 0;

int Scene::createNewId() {
	// Return a new numerical Id
	return ++LastSceneId;
}

Scene::Scene(std::shared_ptr<Renderer> r, ApplicationSettings& appSettings, SceneInformation sceneInfo) {
	renderer = r;
	sceneName = sceneInfo.n;
	myId = sceneInfo.id;
	hasTerrain = sceneInfo.hasTerrain;
	
	dout.log("Scene constructor called");

	// Create the ui
	ui = std::unique_ptr<UIWrangler>(new UIWrangler(renderer, appSettings, sceneName));
	hookClass(ui->getUiEngine()->getState());
	EntityOrders::hookClass(ui->getUiEngine()->getState());

	// Create our loading UI
	initLoadingUi(appSettings);

	// Create the Terrain
	if (hasTerrain) {
		terrain = std::unique_ptr<Terrain>(new Terrain("maps/testMap"));

		if (!terrain->isValid()) {
			dout.error("Terrain is invalid, switching off terrain to prevent issues");
			hasTerrain = false;
		}
	}
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
	renderer->getCamera()->setTacticalZoomParams(14.0f, 1500.0f, 15.0f);
	renderer->getCamera()->update(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);

	setCameraEnabled(true);
	
	// Put a light under the spider at 0,0,0
	renderer->setLightPosition(1, glm::vec3(0, 5.5f, 0)); // Make sure it is above the floor
	renderer->setLightColor(1, glm::vec3(0, 0, 0));
	renderer->setLightAttenuation(1, true);

	// Create the camera light
	renderer->setLightPosition(2, renderer->getCamera()->position);
	renderer->setLightColor(2, glm::vec3(1, 1, 1));
	renderer->setLightAttenuation(2, true);

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

void Scene::initLoadingUi(ApplicationSettings& appSettings) {
	loadingUi = std::unique_ptr<UIWrangler>(new UIWrangler(renderer, appSettings, "loading"));
	tgui::ProgressBar::Ptr bar = loadingUi->getWidgetByName("loadingBar")->cast<tgui::ProgressBar>();
	bar->setPosition("20%", "48%");
	bar->setSize("60%", "4%");
	bar->setMaximum(1000);
	bar->setMinimum(0);

	// Hook the loading percentage into the lua engine
	hookClass(loadingUi->getUiEngine()->getState());
}

void Scene::hookClass(lua::State *L) {
	try {
		luabridge::getGlobalNamespace(L->getState())
			.beginNamespace("darksun")
				.beginClass<Scene>("Scene")
					//.addConstructor<void(*)(std::shared_ptr<Renderer>, ApplicationSettings& settings, string, int), RefCountedPtr<Scene> /* creation policy */ >()
					.addFunction("spawnEntity", &darksun::Scene::spawnEntity)
					.addFunction("killEntity", &darksun::Scene::killEntity)
					.addFunction("getPercentLoaded", &Scene::getTerrainPercentLoaded)
				.endClass()
			.endNamespace();

		// Add this instance
		push(L->getState(), this);
		lua_setglobal(L->getState(), "Scene");
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed Scene exposure proccess: " + what);
		return;
	}
}

void Scene::close() {
	// Do something?
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

	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderer->SCREEN_WIDTH / (float)renderer->SCREEN_HEIGHT, 0.1f, 2000.0f);
	glm::mat4 view = camera->GetViewMatrix();
	//glm::mat4 view = glm::lookAt(camera->Position, glm::vec3(camera->Position.x, 0, camera->Position.z), camera->WorldUp);
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	if (!terrain->isLoaded() && hasTerrain) {
		return;
	}

	// Draw the terrain
	if(hasTerrain)
		terrain->draw(shader);

	// Draw the entities
	for (auto &e : entities) {
		if (e->isValid()) {
			e->draw(shader);
		}
		else {
			dout.warn("Invalid entity detected in scene '" + sceneName + "', attempted to draw. Probably hasn't been collected for garbage yet (entityId = '" + std::to_string(e->getId()) + "')");
		}
	}
}

void Scene::drawUI() {
	if (!terrain->isLoaded() && hasTerrain) {
		loadingUi->draw();
		return;
	}

	// Draw the UI
	ui->draw();
}

void Scene::handleEvent(sf::Event& ev) {
	ui->handleEvent(ev);
}

void Scene::tick(float deltaTime) {

	// Tick the terrain
	if(hasTerrain)
		terrain->tick(deltaTime);

	// check to see if we care about terrain and if it has loaded. If it hasn't do the code
	if (!terrain->isLoaded() && hasTerrain) {
		// Update the progress bar and tick the loading ui
		tgui::ProgressBar::Ptr bar = loadingUi->getWidgetByName("loadingBar")->cast<tgui::ProgressBar>();
		bar->setValue(terrain->getLoadedPercent() * 10);
		loadingUi->tick(deltaTime);
		return;
	}
	
	// Move the light in a circle (for now)
	renderer->setLightPosition(0, glm::vec3(10.0f * sinf(sinArg), 10.0f, 10.0f * cosf(sinArg)));
	renderer->setLightColor(1, glm::vec3(sinf(sinArg), 0.0f, cosf(sinArg)));

	// Move the camera light to below the camera
	renderer->setLightPosition(2, renderer->getCamera()->position);

	sinArg += 0.05;

	ui->tick(deltaTime);

	for (auto& e : entities) {
		// Check for entity failures we need to remove
		if (!e->isValid()) {
			entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
		}
		else {
			e->tick(deltaTime);
		}
	}
}

int Scene::spawnEntity(string bpN, float x, float y, float z) {
	std::shared_ptr<Entity> ent = std::shared_ptr<Entity>(new Entity(bpN));
	
	// Put the entity on the terrain - TODO
	
	ent->setPosition(glm::vec3(x,y,z));

	int entId = ent->getId();
	entities.push_back(ent);

	return entId;
}

void Scene::killEntity(int id) {
	for (auto& ent : entities) {
		if (ent->getId() == id) {
			// Issue a kill order on the entitiy
			ent->kill();
		}
	}
}

void Scene::issueEntityOrder(int id, LuaRef orderTable) {
	if (!orderTable.isTable()) {
		dlua.error("Attempted to issue entity order, but got not a table!");
	}

	// Check for the correct order information
	if (!orderTable["type"].isNumber()) {
		dlua.error("Entity order has non-number order type");
	}
	
	// We have a valid type at least
	int orderType = (int)orderTable["type"];
	EntityOrder order;
	order.type = orderType;
	switch (orderType) {

	case EntityOrders::ORDER_MOVE:
		order.position = glm::vec3(orderTable["position"][1], orderTable["position"][2], orderTable["position"][3]);
		break;

	case EntityOrders::ORDER_ATTACK:

		break;

	// These orders we don't need to do anything for
	case EntityOrders::ORDER_STOP:
		break;

	default:
		dlua.warn("Unknown order type of " + std::to_string(orderType));
		order.type = EntityOrders::ORDER_NONE;
		break;

	}

	for (auto& ent : entities) {
		if (ent->getId() == id) {
			// Issue an order
			//ent->issueOrder(order);
		}
	}
}