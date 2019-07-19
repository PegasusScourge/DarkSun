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

		// Clears the screen
		void clearscreen();

		// Applies the current lighting effects
		void prepLights(std::shared_ptr<Shader> shader);

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
		// sets attenuation on a light
		void setLightAttenuation(int index, bool a) {
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightAttenuates[index] = a;
		}
		// sets if gamma correction is enabled in the shaders
		void setGammaCorrection(std::shared_ptr<Shader> shader, bool g);

		unsigned int getShadowWidth() { return SHADOW_WIDTH; }
		unsigned int getShadowHeight() { return SHADOW_HEIGHT; }
		unsigned int getDepthMapFBO() { 
			return depthMapFBO;
		}
		unsigned int getDepthMap() {
			return depthMap;
		}

		/*
		Destruction
		*/
		void cleanup();

		/*
		Getters
		*/
		sf::RenderWindow* getWindowHandle();

		Camera* getCamera();

	private:

		void initShadows();

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

		// Shadows
		
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		unsigned int depthMapFBO;
		unsigned int depthMap;
		float depthBorderColor[4] = { 1.0, 1.0, 1.0, 1.0 };

	};

}


