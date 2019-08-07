#pragma once
/**

File: Map.hpp
Description:

Header for Map.cpp

THREADING IN OPERATION, might be safe

*/

#include <future>
#include <atomic>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.hpp"
#include "Renderable.hpp"
#include "LuaEngine.hpp"
#include "MultiThreadedOpenGL.hpp"

using namespace darksun;

namespace darksun {

	class Map : public Renderable {

	public:
		Map(string mapfolder);
		~Map() {
			dout.log("Map destructor called");
		}

		bool isValid() { return valid; }

		void tick(float deltaTime);

		float getLoadedPercent() {
			return loadedPercent;
		}

		int getSizeX() {
			return sizeX.load();
		}
		int getSizeY() {
			return sizeY.load();
		}

		glm::vec3 getSunPosition() {
			return sunPosition.load();
		}

		glm::vec3 getSunColor() {
			return sunColor.load();
		}

		bool sunIsSpecified() {
			return sunSpecified.load();
		}

		// Hooks the class to a lua engine
		void hookClass(lua::State* L);

	private:

		struct ProtoTextureInfo {
			string diffuseSrc; 
			bool diffuseGammaCorrection;
		};

		struct LoadingResult {
			std::vector<unsigned int> indiciesBuff;
			std::vector<Vertex> vertexBuff;

			ProtoTextureInfo textInfo;

			int exitValue = -1;
		};

		Texture MapText;

		bool valid = false;
		std::atomic<float> loadedPercent = 0.0f;

		string heightMapLoc;
		string textureLoc;
		string dir;
		string luaLoc;

		// Loading thread info
		std::future<LoadingResult> loadingThreadResult;
		LoadingResult result;

		std::atomic<int> sizeX = 0;
		std::atomic<int> sizeY = 0;
		std::atomic<float> lowestP = 0;
		std::atomic<float> highestP = 0;

		// Lighting information
		std::atomic<bool> sunSpecified = false;
		std::atomic<glm::vec3> sunColor = glm::vec3(0, 0, 0);
		std::atomic<glm::vec3> sunPosition = glm::vec3(0, 0, 0);

		LuaEngine loadingEngine;

		LoadingResult loadMap();
	};

}