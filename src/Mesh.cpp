
/**

File: Mesh.cpp
Description:

Stores information on verticies and indicies and the associated memory in GPU memory

*/

#include "Mesh.hpp"

using namespace darksun;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh() {
	myDef = mtopengl::getVAO(&vertices, &indices);
}

void Mesh::tick(float deltaTime) {
	// Check for VBO updates
	if (updateVBO) {
		updateVBO = false;

		mtopengl::updateVBO(myDef.ref, &vertices);
	}
}