/**

File: Renderer.cpp
Description:

A class that handles a window and the rendering to and from it

*/

#include "Renderer.h"

void Renderer::createWindow(sf::ContextSettings& settings) {
	defaultWindow.create(sf::VideoMode(800, 600), "DarkSun", sf::Style::Default, settings);
}

void Renderer::create() {
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 0;
	createWindow(settings);
}

sf::RenderWindow* Renderer::getWindowHandle() {
	return &defaultWindow;
}

void Renderer::cleanup() {
	defaultWindow.close();
}