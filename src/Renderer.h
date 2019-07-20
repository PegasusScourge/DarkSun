#pragma once
/**

File: Renderer.h
Description:

Header file for Renderer.cpp. A class that handles a window and the rendering to and from it

*/

// Must include first
#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include "Log.h"
#include "Camera.h"
#include "Shader.h"
#include "ApplicationSettings.h"
#include "Renderable.h"
#include "UiHandler.h"

#include "DarkSunProfiler.h"

namespace darksun {

	class Renderer {

	public:
		const int SCREEN_WIDTH = 1768;
		const int SCREEN_HEIGHT = 992;
		const static int NUMBER_OF_LIGHTS = 4; // WARNING: You must update the number of lights the shader can take if you update this value!!!!!

		/*
		Creation
		*/
		// Used to create the necessary resources on open of the program
		void create(ApplicationSettings &settings);
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
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightPositions[index] = p;
		}
		// Returns the position of a light
		glm::vec3 getLightPosition(int index) {
			glm::vec3 p(0,0,0);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return p; } p = lightPositions[index]; return p;
		}
		// sets the color of a light
		void setLightColor(int index, glm::vec3 p) { 
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightColors[index] = p; 
		}
		// Returns the color of a light
		glm::vec3 getLightColor(int index) {
			glm::vec3 p(0, 0, 0);
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return p; } p = lightColors[index]; return p;
		}
		// sets attenuation on a light
		void setLightAttenuation(int index, bool a) {
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightAttenuates[index] = a;
		}
		// Returns the attenutation of a light
		bool getLightAttenuation(int index) {
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return false; } return lightAttenuates[index];
		}
		// sets if gamma correction is enabled in the shaders
		void setGammaCorrection(bool g);

		unsigned int getShadowWidth() { return SHADOW_WIDTH; }
		unsigned int getShadowHeight() { return SHADOW_HEIGHT; }
		unsigned int getDepthMapFBO() { 
			return depthMapFBO;
		}
		unsigned int getDepthMap() {
			return depthMap;
		}

		sf::RenderWindow* getWindowHandle() {
			return &defaultWindow;
		}

		Camera* getCamera() {
			return &camera;
		}

		// Clears the screen
		void clearscreen();

		/*
		Destruction
		*/
		void cleanup();

	private:

		std::map<string, std::shared_ptr<Renderable>> renderables;
		std::map <string, std::shared_ptr<UIWrangler>> renderableUIs;

		sf::RenderWindow defaultWindow;

		Camera camera;

		ApplicationSettings appSettings;

		// lighting
		glm::vec3 lightPositions[NUMBER_OF_LIGHTS] = {
			glm::vec3(-3.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f)
		};
		glm::vec3 lightColors[NUMBER_OF_LIGHTS] = {
			glm::vec3(1.0),
			glm::vec3(0),
			glm::vec3(0),
			glm::vec3(0)
		};

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

		bool gammaCorrection = false;

		// Shadows
		const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
		unsigned int depthMapFBO;
		unsigned int depthMap;
		float depthBorderColor[4] = { 1.0, 1.0, 1.0, 1.0 };

		void catchOpenGLErrors(string ref);

	};

}


