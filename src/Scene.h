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

namespace darksun {

	/* The base scene class, with standard things */
	class Scene {

	private:
		// Static stuff
		static long LastSceneId;

		// Entities in this scene
		std::vector<std::shared_ptr<Entity>> entities;
		//UIOfSomeKind ui;

		// Naming stuff
		long myId;
		string sceneName = "";
		bool valid = false;

		// Tick variables
		float sinArg = 0.0f;

		// Default shader
		Shader defaultShader;

		// Renderer
		std::shared_ptr<Renderer> renderer;

		// UIWrangler
		std::shared_ptr<UIWrangler> ui;

	public:
		static long createNewId();

		Scene(std::shared_ptr<Renderer>, string, long);

		// Draw the scene
		void draw(Shader* shader);

		// Pass events
		void handleEvent(sf::Event& ev);

		// Tick the scene
		void tick();

		// Init function called to start the scene
		void init();

		// Test function that inits some test objects
		void initTest();

		// Close function used to shut down the scene
		void close();

		// Returns the default shader for this scene
		Shader* getDefaultShader() { return &defaultShader; }

		// Returns if we are valid
		bool isValid() { return valid; }

	};

}