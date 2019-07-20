#pragma once

#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <algorithm>


#include "Shader.h"

using string = std::string;
using namespace darksun;

namespace darksun {

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	struct Texture {
		unsigned int id;
		string type;
		string path;
	};

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

