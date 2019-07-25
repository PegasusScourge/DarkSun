#pragma once
/**

File: MultiThreadedOpenGL.h
Description:

Handles all openGL calls and requests to the correct thread

THREADING IN OPERATION, might be safe

*/
#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

#include <SFML/Window.hpp>

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <chrono>
#include "stb_image.h"

#include "Log.h"
#include "DarkSunProfiler.h"
#include "OpenGLStructs.h"

using string = std::string;

namespace darksun::mtopengl {

	// Stores loading information for a Texture struct and texture binding
	struct TextureDef {
		unsigned int id = 0;
		string filename = "";
		bool gamma = false;
	};

	// Stores the information about a VAO
	struct VAODef {
		unsigned int VAO = 0;
		unsigned int VBO = 0; unsigned int VBOSize = 0;
		unsigned int EBO = 0; unsigned int EBOSize = 0;
		int ref = 0;

		std::vector<Vertex> vertices = std::vector<Vertex>();
		std::vector<unsigned int> indices = std::vector<unsigned int>();
	};

	// Contains information to update a VAO buffer: verticies (VBO)
	struct VBOUpdateDef {
		int vaoDefRef = 0;

		std::vector<Vertex> vertices = std::vector<Vertex>();
	};

	// Accessed by the OpenGL thread only
	void process();

	// Accessed by functions that want to get a textre from the multi-threading solution
	unsigned int getTexture(const string filename, bool gamma);

	// Accessed by the opengl thread ONLY
	unsigned int textureFromFile(const string filename, bool gamma);

	// Accessed by the opengl thread ONLY
	void processTextureLoadRequests();

	// Accessed by functions that want to get a VAO from the multi-threading solution
	mtopengl::VAODef getVAO(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices);

	// Accessed by the opengl thread ONLY
	void processVAOLoadRequests();

	// Accessed by main thread functions that want to update their VAO VBO data
	void updateVBO(int vaoRef, std::vector<Vertex>* vertices);

	// Accesed by the opengl thread ONLY
	void processVBOUpdateRequests();

	// Accessed by the main thread to intercept events
	std::vector<sf::Event> getEvents();

	// Accessed by the opengl thread to push events
	void addEvent(sf::Event e);

}