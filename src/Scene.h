#pragma once
/**

File: Scene.h
Description:

Header file for Scene.cpp, defines all scenes

*/

#include <vector>

#include "Entity.h"
#include "Shader.h"
#include "Renderer.h"
#include "Log.h"
#include "UiHandler.h"
#include "Map.h"

#include <TGUI/TGUI.hpp>

namespace darksun {

	struct SceneInformation {
		string n = "testScene";
		int id = 0;
		string mapName = "";
		bool hasMap = false;
	};
	
	/* The base scene class, with standard things */
	class Scene {

	private:
		// Static stuff
		static int LastSceneId;

		// Entities in this scene
		std::vector<std::shared_ptr<Entity>> entities;
		//UIOfSomeKind ui;

		// Naming stuff
		int myId;
		string sceneName = "";
		bool valid = false;

		// Tick variables
		float sinArg = 0.0f;

		bool cameraEnabled = false; // Marks if the camera input should be processed
		bool hasMap = true; // Marks if we should create or care about terrain

		// Default shader
		std::shared_ptr<Shader> defaultShader;
		// Shadow shader
		std::shared_ptr<Shader> shadowShader;

		// Renderer
		std::shared_ptr<Renderer> renderer;

		// UIWrangler
		std::unique_ptr<UIWrangler> ui;

		// UIWrangler
		std::unique_ptr<UIWrangler> loadingUi;

		// Terrain
		std::unique_ptr<Map> map;

		// app settings
		ApplicationSettings appSettings;

		// Hook the Ui with scene functions
		void hookClass(lua::State *L);

		// Lua exposed things
		void lua_setLightPosition(int l, float x, float y, float z) { renderer->setLightPosition(l, glm::vec3(x, y, z)); }
		void lua_setLightColor(int l, float r, float g, float b) { renderer->setLightColor(l, glm::vec3(r, g, b)); }
		void lua_setLightAttenuation(int l, bool a) { renderer->setLightAttenuation(l, a); }
		glm::vec3 lua_getLightPosition(int l) { return renderer->getLightPosition(l); }
		glm::vec3 lua_getLightColor(int l) { return renderer->getLightColor(l); }
		bool lua_getLightAttenuation(int l) { return renderer->getLightAttenuation(l); }

	public:
		static int createNewId();

		Scene(std::shared_ptr<Renderer>, ApplicationSettings& appSettings, SceneInformation sceneInfo);

		// Draw the scene
		void draw(std::shared_ptr<Shader> shader);
		void drawUI();

		// Pass events
		void handleEvent(sf::Event& ev);

		// Tick the scene
		void tick(float deltaTime);

		// Init function called to start the scene
		void init();

		// Inits the loading UI
		void initLoadingUi(ApplicationSettings& appSettings);

		// Close function used to shut down the scene
		void close();

		// Returns the default shader for this scene
		std::shared_ptr<Shader> getDefaultShader() { return defaultShader; }

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
		int spawnEntity(string bpN, float x, float y, float z);

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