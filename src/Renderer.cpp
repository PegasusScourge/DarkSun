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
	glDepthMask(true);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// For gamma correction
	glEnable(GL_FRAMEBUFFER_SRGB);

	// Enable culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Create the shadow stuffs
		// configure depth map FBO
		// -----------------------
		glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depthBorderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Do a glew test:
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	dout.log("glewTest: " + std::to_string(vertexBuffer));
}

void Renderer::clearscreen() {
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::prepLights(std::shared_ptr<Shader> shader) {
	shader->setVec3("objectColor", 1.0f, 1.0f, 1.0f);
	// set light uniforms
	glUniform3fv(glGetUniformLocation(shader->ID, "lightPositions"), NUMBER_OF_LIGHTS, &lightPositions[0][0]);
	glUniform3fv(glGetUniformLocation(shader->ID, "lightColors"), NUMBER_OF_LIGHTS, &lightColors[0][0]);
	glUniform1iv(glGetUniformLocation(shader->ID, "lightAttenuates"), NUMBER_OF_LIGHTS, &lightAttenuates[0]);
	shader->setVec3("viewPos", camera.position);
}

void Renderer::setGammaCorrection(std::shared_ptr<Shader> shader, bool g) {
	shader->setInt("gamma", g);
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