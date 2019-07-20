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
	hasMap = sceneInfo.hasMap;
	
	this->appSettings = appSettings;

	dout.log("Scene constructor called");

	// Create the ui
	ui = std::unique_ptr<UIWrangler>(new UIWrangler(renderer, appSettings, sceneName));
	hookClass(ui->getUiEngine()->getState());
	EntityOrders::hookClass(ui->getUiEngine()->getState());

	ui->OnCreate();

	// Create our loading UI
	initLoadingUi(appSettings);

	// Create the Terrain
	if (hasMap) {
		map = std::shared_ptr<Map>(new Map("maps/testMap"));

		if (!map->isValid()) {
			dout.error("Terrain is invalid, switching off terrain to prevent issues");
			hasMap = false;
		}
		// Register the map with the renderer
		r->registerRenderable("map", std::dynamic_pointer_cast<Renderable>(map));
	}

	init();
}

void Scene::init() {
	dout.log("Creating scene '" + sceneName + "'");

	// Start with gamma correction set off
	renderer->setGammaCorrection(false);

	// Create the camera light
	renderer->setLightPosition(0, renderer->getCamera()->position);
	renderer->setLightColor(0, glm::vec3(0.5f, 0.5f, 0.5f));
	renderer->setLightAttenuation(0, true);

	renderer->getCamera()->setTacticalZoomParams(14.0f, 1500.0f, 15.0f);
	renderer->getCamera()->update(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);

	setCameraEnabled(true);

	dout.log("Scene creation complete");
	valid = true;
}

void Scene::initLoadingUi(ApplicationSettings& appSettings) {
	loadingUi = std::unique_ptr<UIWrangler>(new UIWrangler(renderer, appSettings, "loading"));

	// Hook the loading percentage into the lua engine
	hookClass(loadingUi->getUiEngine()->getState());

	loadingUi->OnCreate();

	tgui::ProgressBar::Ptr bar = loadingUi->getWidgetByName("loadingBar")->cast<tgui::ProgressBar>();
	bar->setPosition("20%", "48%");
	bar->setSize("60%", "4%");
	bar->setMaximum(1000);
	bar->setMinimum(0);
}

void Scene::hookClass(lua::State *L) {
	try {
		luabridge::getGlobalNamespace(L->getState())
			.beginNamespace("darksun")
				.beginClass<Scene>("Scene")
					//.addConstructor<void(*)(std::shared_ptr<Renderer>, ApplicationSettings& settings, string, int), RefCountedPtr<Scene> /* creation policy */ >()
					.addFunction("spawnEntity", &darksun::Scene::spawnEntity)
					.addFunction("killEntity", &darksun::Scene::killEntity)
					.addFunction("getPercentLoaded", &darksun::Scene::getTerrainPercentLoaded)
					.addFunction("setLightPosition", &darksun::Scene::lua_setLightPosition)
					.addFunction("setLightColor", &darksun::Scene::lua_setLightColor)
					.addFunction("setLightAttenuation", &darksun::Scene::lua_setLightAttenuation)
					.addFunction("getLightPosition", &darksun::Scene::lua_getLightPosition)
					.addFunction("getLightColor", &darksun::Scene::lua_getLightColor)
					.addFunction("getLightAttenuation", &darksun::Scene::lua_getLightAttenuation)
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

void Scene::drawUI() {
	if (!map->isLoaded() && hasMap) {
		// Clear the screen to black
		renderer->clearscreen();
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
	if(hasMap)
		map->tick(deltaTime);

	// check to see if we care about terrain and if it has loaded. If it hasn't do the code
	if (!map->isLoaded() && hasMap) {
		// Update the progress bar and tick the loading ui
		tgui::ProgressBar::Ptr bar = loadingUi->getWidgetByName("loadingBar")->cast<tgui::ProgressBar>();
		bar->setValue(map->getLoadedPercent() * 10);
		loadingUi->tick(deltaTime);
		return;
	}

	// Move the camera light to below the camera
	renderer->setLightPosition(0, renderer->getCamera()->position);

	sinArg += 0.05;

	ui->tick(deltaTime);

	for (auto& e : entities) {
		// Check for entity failures we need to remove
		if (!e->isValid()) {
			// Remove the entity from the renderer first!
			renderer->unregisterRenderable("entity" + std::to_string(e->getId()));

			entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
		}
		else {
			e->tick(deltaTime);
		}
	}
}

int Scene::spawnEntity(string bpN, float x, float y, float z) {
	std::shared_ptr<Entity> ent = std::shared_ptr<Entity>(new Entity(bpN));
	
	// Register the entity's model with the renderer
	renderer->registerRenderable("entity" + std::to_string(ent->getId()),ent->getModelPtr());

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