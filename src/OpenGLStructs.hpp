#pragma once
/**

File: OpenGLStructs.hpp
Description:

Basic opengl structs/defs etc

*/

#include <glm/glm.hpp>
#include <string>

using string = std::string;

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

}
