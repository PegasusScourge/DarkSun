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

namespace darksun {

	class Renderer {

	public:
		const int SCREEN_WIDTH = 800;
		const int SCREEN_HEIGHT = 600;
		const static int NUMBER_OF_LIGHTS = 4; // WARNING: You must update the number of lights the shader can take if you update this value!!!!!

		/*
		Creation
		*/
		// Used to create the necessary resources on open of the program
		void create();
		// (Re)Creates the window with the specified settings (passed by reference)
		void createWindow(sf::ContextSettings& settings);

		// Clears the screen
		void clearscreen();

		// Applies the current lighting effects
		void prepLights(Shader& shader);

		// sets the position of the light
		void setLightPosition(int index, glm::vec3 p) { 
			if (index < 0 || index >= NUMBER_OF_LIGHTS) { return; } lightPositions[index] = p;
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
		void setGammaCorrection(Shader& shader, bool g);

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
		sf::RenderWindow defaultWindow;

		Camera camera;

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

	};

}


