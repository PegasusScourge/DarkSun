/**

File: Renderer.cpp
Description:

A class that handles a window and the rendering to and from it

*/

#include "Renderer.h"

using namespace darksun;

void Renderer::createWindow(sf::ContextSettings& settings) {
	defaultWindow.create(sf::VideoMode(800, 600), "DarkSun", sf::Style::Default, settings);
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

	// Do a glew test:
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	dout.log("glewTest: " + std::to_string(vertexBuffer));
}

sf::RenderWindow* Renderer::getWindowHandle() {
	return &defaultWindow;
}

void Renderer::cleanup() {
	defaultWindow.close();
}