#pragma once

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
		/*  Mesh Data  */
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		std::vector<Texture> getTextures() {
			return textures;
		}
		int getNumberOfIndices() {
			return indices.size();
		}
		
		// This is accessed through the OpenGL thread so IS FINE RIGHT HERE
		void GL_bindVertexArray() {
			glBindVertexArray(VAO);
		}

		/*  Functions  */
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	private:
		/*  Render data  */
		unsigned int VAO, VBO, EBO;

		/*  Functions    */
		void setupMesh();
	};

}

