/**

File: Renderer.cpp
Description:

A class that handles a window and the rendering to and from it

*/

#include "Renderer.h"

using namespace darksun;

void Renderer::createWindow(sf::ContextSettings& settings) {
	defaultWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "DarkSun", sf::Style::Default, settings);
}

void Renderer::create() {
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 2;
	createWindow(settings);

	// Now we have a context, init glew
	glewExperimental = GL_TRUE;
	glewInit();

	// Do state init for opengl
	glEnable(GL_DEPTH_TEST);

	// Do a glew test:
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	dout.log("glewTest: " + std::to_string(vertexBuffer));
}

void Renderer::clearscreen() {
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

sf::RenderWindow* Renderer::getWindowHandle() {
	return &defaultWindow;
}

Camera* Renderer::getCamera() {
	return &camera;
}

void Renderer::cleanup() {
	defaultWindow.close();
}