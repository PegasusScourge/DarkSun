#pragma once
/**

File: Renderer.hpp
Description:

Header file for Renderer.cpp. A class that handles a window and the rendering to and from it

Attempted thread safety

*/

// Must include first
#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <atomic>
#include <mutex>

#include "Log.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "ApplicationSettings.hpp"
#include "Renderable.hpp"
#include "UiHandler.hpp"

#include "DarkSunProfiler.hpp"

namespace darksun {

	class Renderer {

	public:
		const int SCREEN_WIDTH = 1768;
		const int SCREEN_HEIGHT = 992;
		const static int NUMBER_OF_LIGHTS = 4; // WARNING: You must update the number of lights the shader can take if you update this value!!!!!

		/*
		Creation
		*/
		Renderer() {
			
		}
		// Used to create the necessary resources on open of the program
		void create(ApplicationSettings* settings);
		// (Re)Creates the window with the specified settings (passed by reference)
		void createWindow(sf::ContextSettings& settings);

		// Draws all registered Renderables
		void render();

		// Registers renderables
		void registerRenderable(string name, std::shared_ptr<Renderable> n);

		// Unregisteres a renderable
		void unregisterRenderable(string name);

		// Registers uis
		void registerUI(string name, std::shared_ptr<UIWrangler> n);

		// Unregisteres a ui
		void unregisterUI(string name);

		// sets the position of the light
		void setLightPosition(int index, glm::vec3 p) { 
			std::lock_guard lock(lightPositions_mutex);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightPositions[index] = p;
		}
		// Returns the position of a light
		glm::vec3 getLightPosition(int index) {
			std::lock_guard lock(lightPositions_mutex);
			glm::vec3 p(0,0,0);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return p; } p = lightPositions[index]; return p;
		}
		// sets the color of a light
		void setLightColor(int index, glm::vec3 p) { 
			std::lock_guard lock(lightColors_mutex);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightColors[index] = p; 
		}
		// Returns the color of a light
		glm::vec3 getLightColor(int index) {
			std::lock_guard lock(lightColors_mutex);
			glm::vec3 p(0, 0, 0);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return p; } p = lightColors[index]; return p;
		}
		// sets attenuation on a light
		void setLightAttenuation(int index, bool a) {
			std::lock_guard lock(lightAttenuates_mutex);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightAttenuates[index] = a;
		}
		// Returns the attenutation of a light
		bool getLightAttenuation(int index) {
			std::lock_guard lock(lightAttenuates_mutex);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return false; } return lightAttenuates[index];
		}
		// sets if gamma correction is enabled in the shaders
		void setGammaCorrection(bool g);

		unsigned int getShadowWidth() { return SHADOW_WIDTH; }
		unsigned int getShadowHeight() { return SHADOW_HEIGHT; }
		unsigned int getDepthMapFBO() { 
			std::lock_guard lock(depthMapFBO_mutex);
			return depthMapFBO;
		}
		unsigned int getDepthMap() {
			std::lock_guard lock(depthMap_mutex);
			return depthMap;
		}

		sf::RenderWindow* getWindowHandle() {
			std::lock_guard lock(defaultWindow_mutex);
			return &defaultWindow;
		}

		std::shared_ptr<Camera> getCamera() {
			std::lock_guard lock(camera_mutex);
			return camera;
		}

		// Clears the screen
		void clearscreen();

		/*
		Destruction
		*/
		void cleanup();

	private:

		std::mutex renderables_mutex;
		std::map<string, std::shared_ptr<Renderable>> renderables;
		std::mutex renderableUIs_mutex;
		std::map <string, std::shared_ptr<UIWrangler>> renderableUIs;

		std::mutex defaultWindow_mutex;
		sf::RenderWindow defaultWindow;

		std::mutex camera_mutex;
		std::shared_ptr<Camera> camera; // we do nothing to protect the thread safety of camera, only to handle the thread safety of the pointer

		// Is thread safed itself, no one else accesses this copy
		ApplicationSettings* appSettings;

		// lighting
		std::mutex lightPositions_mutex;
		glm::vec3 lightPositions[NUMBER_OF_LIGHTS] = {
			glm::vec3(-3.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f)
		};
		std::mutex lightColors_mutex;
		glm::vec3 lightColors[NUMBER_OF_LIGHTS] = {
			glm::vec3(1.0),
			glm::vec3(0),
			glm::vec3(0),
			glm::vec3(0)
		};
		std::mutex lightAttenuates_mutex;
		int lightAttenuates[NUMBER_OF_LIGHTS] = {
			true,
			true,
			true,
			true
		};

		// Applies the current lighting effects
		void prepLights(std::shared_ptr<Shader> shader);

		// Inits the shadow buffers
		void initShadows();

		// Inits the shaders
		void initShaders();

		// Draws the scene
		void draw(std::shared_ptr<Shader> shader);

		// Draws the UI
		void drawUi();

		// Default shader
		std::shared_ptr<Shader> defaultShader;
		// Shadow shader
		std::shared_ptr<Shader> defaultShadowShader;

		std::atomic<bool> gammaCorrection = false;

		// Shadows
		const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
		std::mutex depthMapFBO_mutex;
		unsigned int depthMapFBO;
		std::mutex depthMap_mutex;
		unsigned int depthMap;
		float depthBorderColor[4] = { 1.0, 1.0, 1.0, 1.0 };

		void catchOpenGLErrors(string ref);

	};

}


