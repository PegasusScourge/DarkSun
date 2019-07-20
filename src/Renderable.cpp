/**

File: Renderable.cpp
Description:

Base class for all objects that can be rendered

Exposed from main thread to Renderer opengl thread, needs thread safety

*/

#include "Renderable.h"

// Constructor
Renderable::Renderable() {
}

void Renderable::addMesh(Mesh m) {
	std::lock_guard<std::mutex> lock(meshesMutex);
	meshes.push_back(m);
}

//std::vector<Mesh> Renderable::getMeshes() {
//	std::vector<Mesh> ret;
//	meshesMutex.lock();
//	ret = meshes;
//	meshesMutex.unlock();
//	return ret;
//}

int Renderable::getNumberOfMeshes() {
	std::lock_guard<std::mutex> lock(meshesMutex);
	return meshes.size();
}

Mesh Renderable::getMeshAt(int index) {
	std::lock_guard<std::mutex> lock(meshesMutex);
	return meshes[index];
}

void Renderable::setPosition(float x, float y, float z) {
	this->setPosition(glm::vec3(x, y, z));
}

void Renderable::setPosition(glm::vec3 n) {
	position.store(n);
}

void Renderable::setRotation(float x, float y, float z) {
	this->setRotation(glm::vec3(x, y, z));
}

void Renderable::setRotation(glm::vec3 n) {
	rotation.store(n);
}

void Renderable::setScale(float x, float y, float z) {
	this->setScale(glm::vec3(x, y, z));
}

void Renderable::setScale(float s) {
	this->setScale(glm::vec3(s, s, s));
}

void Renderable::setScale(glm::vec3 n) {
	scale.store(n);
}