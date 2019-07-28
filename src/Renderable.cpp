/**

File: Renderable.cpp
Description:

Base class for all objects that can be rendered

Exposed from main thread to Renderer opengl thread, needs thread safety

Thread safed

*/

#include "Renderable.hpp"

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
	profiler::ScopeProfiler myProfiler("Renderable.cpp::Renderable::getNumberOfMeshes()");
	std::lock_guard<std::mutex> lock(meshesMutex);
	return meshes.size();
}

Mesh& Renderable::getMeshAt(int index) {
	profiler::ScopeProfiler myProfiler("Renderable.cpp::Renderable::getMeshAt()");
	std::lock_guard<std::mutex> lock(meshesMutex);
	return meshes[index];
}

void Renderable::setPosition(float x, float y, float z) {
	this->setPosition(glm::vec3(x, y, z));
}

void Renderable::setPosition(glm::vec3 n) {
	profiler::ScopeProfiler myProfiler("Renderable.cpp::Renderable::setPosition()");
	position.store(n);
}

void Renderable::setRotation(float x, float y, float z) {
	this->setRotation(glm::vec3(x, y, z));
}

void Renderable::setRotation(glm::vec3 n) {
	profiler::ScopeProfiler myProfiler("Renderable.cpp::Renderable::setRotation()");
	rotation.store(n);
}

void Renderable::setScale(float x, float y, float z) {
	this->setScale(glm::vec3(x, y, z));
}

void Renderable::setScale(float s) {
	this->setScale(glm::vec3(s, s, s));
}

void Renderable::setScale(glm::vec3 n) {
	profiler::ScopeProfiler myProfiler("Renderable.cpp::Renderable::setScale()");
	scale.store(n);
}