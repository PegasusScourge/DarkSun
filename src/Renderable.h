#pragma once
/**

File: Renderable.h
Description:

Base class for all objects that can be rendered

Exposed from main thread to Renderer opengl thread, needs thread safety

Thread safed

*/

#include "Mesh.h"
#include <atomic>
#include <mutex>

#include "DarkSunProfiler.h"

using namespace darksun;

namespace darksun {

	class Renderable {

	private:
		std::vector<Mesh> meshes;
		std::mutex meshesMutex;

		std::atomic<bool> gammaCorrection = false;
		std::atomic<bool> loaded = false;

		std::atomic <glm::vec3> position = glm::vec3(0.0f, 0.0f, 0.0f);
		std::atomic <glm::vec3> rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		std::atomic <glm::vec3> scale = glm::vec3(1.0f, 1.0f, 1.0f);

	public:

		// Constructor
		Renderable();

		// Destructor
		~Renderable() {}

		// Tick function
		void tick(float deltaTime) {
			std::lock_guard lock(meshesMutex);
			// Allow the meshes to update
			for (auto& e : meshes) {
				e.tick(deltaTime);
			}
		}

		// Get the position
		glm::vec3 getPosition() { profiler::ScopeProfiler myProfiler("Renderable.h::Renderable::getPosition()"); return position.load(); }
		// Get the rotation
		glm::vec3 getRotation() { profiler::ScopeProfiler myProfiler("Renderable.h::Renderable::getRotation()");return rotation.load(); }
		// Get the scale
		glm::vec3 getScale() { profiler::ScopeProfiler myProfiler("Renderable.h::Renderable::getScale()");return scale.load(); }
		// Get gamma correction
		bool getGammaCorrection() { profiler::ScopeProfiler myProfiler("Renderable.h::Renderable::getGammaCorrection()");return gammaCorrection.load(); }
		// Get the meshes
		//std::vector<Mesh> getMeshes();
		// Get number of meshes
		int getNumberOfMeshes();
		// Get a specific mesh
		Mesh& getMeshAt(int index);

		// Add a mesh to the vector
		void addMesh(Mesh m);

		// Set the postion
		void setPosition(float x, float y, float z); void setPosition(glm::vec3 n);
		// Set the rotation
		void setRotation(float x, float y, float z); void setRotation(glm::vec3 n);
		// Set the scale
		void setScale(float x, float y, float z); void setScale(float s); void setScale(glm::vec3 n);
		// Set the gamma correction
		void setGammaCorrection(bool g) { profiler::ScopeProfiler myProfiler("Renderable.h::Renderable::setGammaCorrection()"); gammaCorrection.store(g); }

		bool isLoaded() {
			return loaded.load();
		}
		void setLoaded(bool l) {
			loaded.store(l);
		}

	};

}