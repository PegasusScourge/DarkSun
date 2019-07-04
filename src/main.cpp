/**

File: main.cpp
Description:

Entry point of the application (main())

*/

#include "Renderer.h"

#include <SFML/Graphics.hpp>

int main()
{
	// Init our rendering
	Renderer renderer;
	renderer.create();
	
	// Test shapes
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	sf::RenderWindow * window = renderer.getWindowHandle();

	bool running = true;
	while (running)
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				running = false;
		}

		window->clear();
		window->draw(shape);
		window->display();
	}

	renderer.cleanup();

	return 0;
}