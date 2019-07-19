#pragma once
/**

File: Map.h
Description:

Header for Map.cpp

*/

#include <future>
#include <atomic>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "LuaEngine.h"

using namespace darksun;

namespace darksun {

	class Map {

	public:
		Map(string mapfolder);
		~Map() {
			dout.log("Map destructor called");
		}

		bool isValid() { return valid; }

		void draw(std::shared_ptr<Shader> shader);

		void tick(float deltaTime);

		bool isLoaded() {
			return loaded;
		}

		float getLoadedPercent() {
			return loadedPercent;
		}

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

		std::unique_ptr<Mesh> MapMesh;

		bool valid = false;
		bool loaded = false;
		std::atomic<float> loadedPercent = 0.0f;

		bool gammaCorrection = false;

		string heightMapLoc;
		string textureLoc;
		string dir;
		string luaLoc;

		// Loading thread info
		std::future<LoadingResult> loadingThreadResult;
		LoadingResult result;

		std::atomic<int> sizeX = 0;
		std::atomic<int> sizeY = 0;

		LuaEngine engine;

		unsigned int TextureFromFile(const string filename, bool gamma);

		LoadingResult loadMap();
	};

}