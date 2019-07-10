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

void Renderer::create(ApplicationSettings &settings) {

	sf::ContextSettings s;
	s.depthBits = settings.opengl_depthBits;
	s.stencilBits = settings.opengl_stencilBits;
	s.antialiasingLevel = settings.opengl_antialiasingLevel;
	s.majorVersion = settings.opengl_majorVersion;
	s.minorVersion = settings.opengl_minorVersion;
	createWindow(s);

	// Now we have a context, init glew
	glewExperimental = GL_TRUE;
	glewInit();

	// Do state init for opengl
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// For gamma correction
	glEnable(GL_FRAMEBUFFER_SRGB);

	// Do a glew test:
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	dout.log("glewTest: " + std::to_string(vertexBuffer));
}

void Renderer::clearscreen() {
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::prepLights(Shader* shader) {
	shader->setVec3("objectColor", 1.0f, 1.0f, 1.0f);
	// set light uniforms
	glUniform3fv(glGetUniformLocation(shader->ID, "lightPositions"), NUMBER_OF_LIGHTS, &lightPositions[0][0]);
	glUniform3fv(glGetUniformLocation(shader->ID, "lightColors"), NUMBER_OF_LIGHTS, &lightColors[0][0]);
	glUniform1iv(glGetUniformLocation(shader->ID, "lightAttenuates"), NUMBER_OF_LIGHTS, &lightAttenuates[0]);
	shader->setVec3("viewPos", camera.position);
}

void Renderer::setGammaCorrection(Shader& shader, bool g) {
	shader.setInt("gamma", g);
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