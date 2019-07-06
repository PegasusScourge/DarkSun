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

	protected:
		// Renderer
		std::shared_ptr<Renderer> renderer;

	public:
		static long createNewId();

		Scene(std::shared_ptr<Renderer> r, string n, long nid = createNewId()) : renderer(r), sceneName(n), myId(nid) { dout.log("Scene constructor called"); };

		// Draw the scene
		void draw(Shader* shader);

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