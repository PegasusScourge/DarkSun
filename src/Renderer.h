#pragma once
/**

File: Renderer.h
Description:

Header file for Renderer.cpp. A class that handles a window and the rendering to and from it

*/

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

class Renderer {

private:
	sf::RenderWindow defaultWindow;

public:
	/* 
	Creation
	*/
	// Used to create the necessary resources on open of the program
	void create();
	// (Re)Creates the window with the specified settings (passed by reference)
	void createWindow(sf::ContextSettings& settings);

	/*
	Destruction
	*/
	void cleanup();

	/*
	Getters
	*/
	sf::RenderWindow* getWindowHandle();

};
