#pragma once

/**

File: Mesh.h
Description:

Stores information on verticies and indicies and the associated memory in GPU memory

*/

#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <algorithm>

#include "Shader.h"
#include "MultiThreadedOpenGL.h"

#include "OpenGLStructs.h"

using string = std::string;
using namespace darksun;

namespace darksun {

	class Mesh {
	public:
		std::vector<Texture> getTextures() {
			return textures;
		}
		int getNumberOfIndices() {
			return indices.size();
		}
		
		// This is accessed through the OpenGL thread so IS FINE RIGHT HERE
		void GL_bindVertexArray() {
			glBindVertexArray(myDef.VAO);
		}

		void deformVertexPosition(int vertIndex, glm::vec3 amount) {
			if (vertIndex < vertices.size() && vertIndex > 0) {
				vertices[vertIndex].Position += amount;
				updateVBO = true;
			}
		}

		// tick function
		void tick(float deltaTime);

		// Constructor
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	private:
		/*  Render data  */
		mtopengl::VAODef myDef;

		/*  Mesh Data  */
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		bool updateVBO = false;

		// Construct the mesh
		void setupMesh();
	};

}

