/**

File: Renderer.cpp
Description:

A class that handles a window and the rendering to and from it

*/

#include "Renderer.hpp"

using namespace darksun;

void Renderer::createWindow(sf::ContextSettings& settings) {
	defaultWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "DarkSun", sf::Style::Default, settings);
}

void Renderer::create(ApplicationSettings* settings) {

	appSettings = settings;

	sf::ContextSettings s;
	s.depthBits = settings->get_opengl_depthBits();
	s.stencilBits = settings->get_opengl_stencilBits();
	s.antialiasingLevel = settings->get_opengl_antialiasingLevel();
	s.majorVersion = settings->get_opengl_majorVersion();
	s.minorVersion = settings->get_opengl_minorVersion();
	createWindow(s);
	defaultWindow.setActive();

	// Now we have a context, init glew
	glewExperimental = GL_TRUE;
	glewInit();

	catchOpenGLErrors("GLEW_INIT");

	// Do state init for opengl
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0, 1);

	catchOpenGLErrors("DEPTH_TEST setup");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	catchOpenGLErrors("BLEND setup");

	// For gamma correction
	glEnable(GL_FRAMEBUFFER_SRGB);

	catchOpenGLErrors("FRAMEBUFFER_SRGB setup");

	// Enable culling
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	catchOpenGLErrors("CULL_FACE setup");

	// Init the shaders
	initShaders();

	// Create the shadow stuffs
	initShadows();

	catchOpenGLErrors("SHADOWS setup");

	// Create camera
	{
		std::lock_guard lock(camera_mutex);
		camera = std::shared_ptr<Camera>(new Camera());
	}
	catchOpenGLErrors("CAMERA setup");

	// Do a glew test:
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	dout.log("glewTest: " + std::to_string(vertexBuffer));
}

void Renderer::initShadows() {
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

	if (depthMap == 0) {
		dout.error("depthMap object is null!");
	}
	if (depthMapFBO == 0) {
		dout.error("depthMapFBO object is null!");
	}
}

void Renderer::initShaders() {
	// Create the shader for directional lights
	defaultShader = std::shared_ptr<Shader>(new Shader("core/shader/lighting_vertex.shader", "core/shader/lighting_geometry.shader", "core/shader/lighting_fragment.shader"));
	defaultShader->use();
	catchOpenGLErrors("defaultShader setup");
	defaultShader->setInt("shadowMap", 10);
	catchOpenGLErrors("shadowMap setup");

	// Create the shadow shader for directional lights
	defaultShadowShader = std::shared_ptr<Shader>(new Shader("core/shader/shadowDepth_vertex.shader", "core/shader/shadowDepth_fragment.shader"));
	catchOpenGLErrors("defaultShadowShader setup");

	if (defaultShader->ID == NULL) {
		dout.error("DEFAULTSHADER == NULL");
	}
	if (defaultShadowShader->ID == NULL) {
		dout.error("DEFAULTSHADOWSHADER == NULL");
	}
}

void Renderer::clearscreen() {
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::prepLights(std::shared_ptr<Shader> shader) {
	shader->setVec3("objectColor", 1.0f, 1.0f, 1.0f);
	// set light uniforms
	{
		std::scoped_lock lock(lightPositions_mutex, lightColors_mutex, lightAttenuates_mutex);
		glUniform3fv(glGetUniformLocation(shader->ID, "lightPositions"), NUMBER_OF_LIGHTS, &lightPositions[0][0]);
		glUniform3fv(glGetUniformLocation(shader->ID, "lightColors"), NUMBER_OF_LIGHTS, &lightColors[0][0]);
		glUniform1iv(glGetUniformLocation(shader->ID, "lightAttenuates"), NUMBER_OF_LIGHTS, &lightAttenuates[0]);

	}
	shader->setVec3("viewPos", camera->getPosition());
}

void Renderer::setGammaCorrection(bool g) {
	gammaCorrection = g;
}

void Renderer::catchOpenGLErrors(string ref) {
	// Catch our own GL errors, if for some reason we create them
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		string errS = "Unknown";
		switch (error) {
		case GL_INVALID_ENUM:
			errS = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errS = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errS = "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errS = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			errS = "GL_OUT_OF_MEMORY";
			break;
		case GL_STACK_UNDERFLOW:
			errS = "GL_STACK_UNDERFLOW";
			break;
		case GL_STACK_OVERFLOW:
			errS = "GL_STACK_OVERFLOW";
			break;
		}

		dout.error("Detected GL error: '" + errS + "' with ref " + ref);
	}
}

void Renderer::cleanup() {
	defaultWindow.close();
}

void Renderer::registerRenderable(string name, std::shared_ptr<Renderable> n) {
	std::lock_guard lock(renderables_mutex);
	// Check for trying to overwrite a previous renderable
	if (renderables.count(name) > 0) {
		dout.error("Tried to register renderable with name=\"" + name + "\", but one with that name is already registered!");
		return;
	}
	
	renderables[name] = n;
}

void Renderer::unregisterRenderable(string name) {
	std::lock_guard lock(renderables_mutex);
	renderables.erase(name);
}

void Renderer::registerUI(string name, std::shared_ptr<UIWrangler> n) {
	std::lock_guard lock(renderableUIs_mutex);
	// Check for trying to overwrite a previous renderable
	if (renderableUIs.count(name) > 0) {
		dout.error("Tried to register UI with name=\"" + name + "\", but one with that name is already registered!");
		return;
	}

	renderableUIs[name] = n;
}

void Renderer::unregisterUI(string name) {
	std::lock_guard lock(renderableUIs_mutex);
	renderableUIs.erase(name);
}

void Renderer::drawUi() {
	profiler::ScopeProfiler drawProfiler("Renderer.cpp::Renderer::drawUi()");
	defaultWindow.pushGLStates();
	for (auto const& e : renderableUIs) {
		e.second->draw();
	}
	defaultWindow.popGLStates();
}

void Renderer::draw(std::shared_ptr<Shader> shader) {
	profiler::ScopeProfiler drawProfiler("Renderer.cpp::Renderer::draw()");

	//dout.verbose("draw()");
	
	int numRenderables = renderables.size();
	int numMeshes = 0;
	for (auto const& r : renderables) {
		if (!r.second->isLoaded()) {
			// This renderable isn't ready to be drawn, skip
			continue;
		}
		
		numMeshes = r.second->getNumberOfMeshes();
		// render the loaded model
		glm::mat4 modelm = glm::mat4(1.0f);
		modelm = glm::translate(modelm, r.second->getPosition());
		modelm = glm::scale(modelm, r.second->getScale());
		modelm = glm::rotate(modelm, glm::radians(r.second->getRotation().x), glm::vec3(1.0f, 0.0f, 0.0f)); //X
		modelm = glm::rotate(modelm, glm::radians(r.second->getRotation().y), glm::vec3(0.0f, 1.0f, 0.0f)); //Y
		modelm = glm::rotate(modelm, glm::radians(r.second->getRotation().z), glm::vec3(0.0f, 0.0f, 1.0f)); //Z
		shader->setMat4("model", modelm);
		
		for (int i = 0; i < numMeshes; i++) {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			auto textures = r.second->getMeshAt(i).getTextures();
			auto numIndicies = r.second->getMeshAt(i).getNumberOfIndices();
			for (unsigned int i = 0; i < std::min((int)textures.size(), 9); i++) {
				//dout.verbose("Binding texture " + std::to_string(i));
				glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
				// retrieve texture number (the N in diffuse_textureN)
				catchOpenGLErrors("Texture select on mesh " + std::to_string(i));

				string number;
				string name = textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);

				shader->setInt(("material." + name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
				catchOpenGLErrors("Texture bind on mesh " + std::to_string(i));
			}
			// Bind the shadow map
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, getDepthMap());
			catchOpenGLErrors("DepthMap bind");

			// draw mesh
			r.second->getMeshAt(i).GL_bindVertexArray();
			catchOpenGLErrors("VBO bind on mesh " + std::to_string(i));
			glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, 0);
			catchOpenGLErrors("Draw on mesh " + std::to_string(i));
			glBindVertexArray(0);
		}
	}
}

void Renderer::render() {
	// Lock the renderables and renderableUIs
	std::scoped_lock lock(renderables_mutex, renderableUIs_mutex);

	profiler::ScopeProfiler renderProfiler("Renderer.cpp::Renderer::render()");

	//dout.verbose("render()");

	// We render shadows
	//dout.verbose("defaultShadowShader use");
	defaultShadowShader->use();

	float near_plane = 0.1f, far_plane = 2.0f;
	float lightProjMult = 1.0f;
	glm::mat4 lightProjection = glm::ortho(-1.0f * lightProjMult, 1.0f * lightProjMult, -1.0f * lightProjMult, 1.0f * lightProjMult, near_plane, far_plane);

	glViewport(0, 0, getShadowWidth(), getShadowHeight());

	// Set the light view to LIGHT 1, only light 1 casts shadows
	glm::vec3 lightPos = getLightPosition(1);
	glm::vec3 lookingAt = glm::vec3(lightPos.x, 0, lightPos.z);
	glm::mat4 lightView = glm::lookAt(lightPos, lookingAt, glm::vec3(0.0f, 1.0f, 0.0f));
	// Create the light space matrix
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	// Pass the space matrix to the shadow shader
	defaultShadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, getDepthMapFBO());
	glClear(GL_DEPTH_BUFFER_BIT);

	catchOpenGLErrors("DepthMapFBO bind");

	// Render scene to shadow buffer
	draw(defaultShadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Return the viewport to its original
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Pass the space matrix to the drawing shader
	//dout.verbose("defaultShader use");
	defaultShader->use();
	defaultShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	catchOpenGLErrors("lightSpaceMatrix bind");

	// Clear the screen to black
	clearscreen();

	// Put in the lighting info
	prepLights(defaultShader);

	catchOpenGLErrors("Light bind");

	// view/projection matricies input

	glm::mat4 projection = glm::perspective(glm::radians(camera->getZoom()), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, appSettings->opengl_nearZ.load(), appSettings->opengl_farZ.load());
	glm::mat4 view = camera->GetViewMatrix();
	//glm::mat4 view = glm::lookAt(camera->Position, glm::vec3(camera->Position.x, 0, camera->Position.z), camera->WorldUp);
	defaultShader->setMat4("projection", projection);
	defaultShader->setMat4("view", view);
	catchOpenGLErrors("Mat4s bind");

	// Draw again
	draw(defaultShader);

	// Draw the UI
	drawUi();
}