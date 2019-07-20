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

	appSettings = settings;

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

	// Init the shaders
	initShaders();

	// Create the shadow stuffs
	initShadows();

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
	// Check for trying to overwrite a previous renderable
	if (renderables.count(name) > 0) {
		dout.error("Tried to register renderable with name=\"" + name + "\", but one with that name is already registered!");
		return;
	}
	
	renderables[name] = n;
}

void Renderer::unregisterRenderable(string name) {
	renderables.erase(name);
}

void Renderer::initShaders() {
	// Create the shader for directional lights
	defaultShader = std::shared_ptr<Shader>(new Shader("core/shader/lighting_vertex.shader", "core/shader/lighting_geometry.shader", "core/shader/lighting_fragment.shader"));
	defaultShader->setInt("shadowMap", 10);
	// Create the shadow shader for directional lights
	defaultShadowShader = std::shared_ptr<Shader>(new Shader("core/shader/shadowDepth_vertex.shader", "core/shader/shadowDepth_fragment.shader"));
}

void Renderer::draw(std::shared_ptr<Shader> shader) {
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
				catchOpenGLErrors("Mesh::draw(1)");

				string number;
				string name = textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);

				shader->setInt(("material." + name + number).c_str(), i);
				catchOpenGLErrors("Mesh::draw(2)");
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
				catchOpenGLErrors("Mesh::draw(3)");
			}
			// Bind the shadow map
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, getDepthMap());

			// draw mesh
			r.second->getMeshAt(i).GL_bindVertexArray();
			catchOpenGLErrors("Mesh::draw(4)");
			glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, 0);
			catchOpenGLErrors("Mesh::draw(5)");
			glBindVertexArray(0);
			catchOpenGLErrors("Mesh::draw(6)");
		}
	}
}

void Renderer::render() {
	// We render shadows
	defaultShadowShader->use();

	float near_plane = 0.1f, far_plane = 2.0f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	glViewport(0, 0, getShadowWidth(), getShadowHeight());

	// Set the light view to LIGHT 1, only light 1 casts shadows
	glm::mat4 lightView = glm::lookAt(getLightPosition(1), glm::vec3(0, 0.0f, 0), glm::vec3(0.0f, 1.0f, 0.0f));
	// Create the light space matrix
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	// Pass the space matrix to the shadow shader
	defaultShadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, getDepthMapFBO());
	glClear(GL_DEPTH_BUFFER_BIT);

	// Render scene to shadow buffer

	draw(defaultShadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Return the viewport to its original
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Pass the space matrix to the drawing shader
	defaultShader->use();
	defaultShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	// Clear the screen to black
	clearscreen();

	// Put in the lighting info
	prepLights(defaultShader);

	// view/projection matricies input

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, appSettings.opengl_nearZ, appSettings.opengl_farZ);
	glm::mat4 view = camera.GetViewMatrix();
	//glm::mat4 view = glm::lookAt(camera->Position, glm::vec3(camera->Position.x, 0, camera->Position.z), camera->WorldUp);
	defaultShader->setMat4("projection", projection);
	defaultShader->setMat4("view", view);

	catchOpenGLErrors("a");

	// Draw again
	draw(defaultShader);

	catchOpenGLErrors("c");
}