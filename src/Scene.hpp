#pragma once
/**

File: Scene.hpp
Description:

Header file for Scene.cpp, defines all scenes

*/

#include <vector>

#include "Entity.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Log.hpp"
#include "UiHandler.hpp"
#include "Map.hpp"

#include <TGUI/TGUI.hpp>

namespace darksun {

	struct SceneInformation {
		string n = "testScene";
		int id = 0;
		string mapName = "";
		bool hasMap = false;
	};

	struct EntityCreateInfo {
		string bp = "";
		float x = 0;
		float y = 0;
		float z = 0;
		int wantedId = 0;
	};
	
	/* The base scene class, with standard things */
	class Scene {

	private:
		// Static stuff
		static int LastSceneId;

		// Entities in this scene
		std::vector<std::shared_ptr<Entity>> entities;
		std::vector<EntityCreateInfo> entitiesToCreate;

		// Naming stuff
		int myId;
		string sceneName = "";
		bool valid = false;

		// Tick variables
		float sinArg = 0.0f;

		bool cameraEnabled = false; // Marks if the camera input should be processed
		bool hasMap = true; // Marks if we should create or care about terrain
		bool switchedUi = false;

		// Renderer
		std::shared_ptr<Renderer> renderer;

		// UIWrangler
		std::shared_ptr<UIWrangler> ui;

		// UIWrangler
		std::shared_ptr<UIWrangler> loadingUi;

		// Terrain
		std::shared_ptr<Map> map;

		// app settings
		ApplicationSettings* appSettings;

		// Hook the Ui with scene functions
		void hookClass(lua::State *L);

		// Process requests to spawn entities
		void processSpawnEntityRequests();

		// Lua exposed things
		void lua_setLightPosition(int l, float x, float y, float z) { renderer->setLightPosition(l, glm::vec3(x, y, z)); }
		void lua_setLightColor(int l, float r, float g, float b) { renderer->setLightColor(l, glm::vec3(r, g, b)); }
		void lua_setLightAttenuation(int l, bool a) { renderer->setLightAttenuation(l, a); }
		glm::vec3 lua_getLightPosition(int l) { return renderer->getLightPosition(l); }
		glm::vec3 lua_getLightColor(int l) { return renderer->getLightColor(l); }
		bool lua_getLightAttenuation(int l) { return renderer->getLightAttenuation(l); }
		void lua_setCameraEnabled(bool a) { setCameraEnabled(a); }
		void lua_setTacticalZoomSettings(float min, float max, float xDelta) { 
			renderer->getCamera()->setTacticalZoomParams(min, max, xDelta);
			renderer->getCamera()->update(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
		}
		bool lua_hasMap() { return hasMap; }

	public:
		static int createNewId();

		Scene(std::shared_ptr<Renderer>, ApplicationSettings* appSettings, SceneInformation sceneInfo);

		// Pass events
		void handleEvent(sf::Event& ev);

		// Tick the scene
		void tick(float deltaTime);

		// Init function called to start the scene
		void init();

		// Inits the loading UI
		void initLoadingUi();

		// Close function used to shut down the scene
		void close();

		// Returns if we are valid
		bool isValid() { return valid; }

		// Enable/disable the camera
		bool isCameraEnabled() { return cameraEnabled; }
		void setCameraEnabled(bool m) { cameraEnabled = m; };

		// Should we transition to next scene
		bool shouldTransition() { return ui->shouldTransition(); }
		// Get transition target
		string getNewScene() { return ui->getNewScene(); }

		// Entity spawning
		void spawnEntity(string bpN, float x, float y, float z);

		// Entity removal
		void killEntity(int id);

		// entity order issuing
		void issueEntityOrder(int id, LuaRef order);

		// Expose the loaded percent of the terrain
		float getTerrainPercentLoaded() {
			return map->getLoadedPercent();
		}

	};

}