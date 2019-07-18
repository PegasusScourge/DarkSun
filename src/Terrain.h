#pragma once
/**

File: Terrain.h
Description:

Header for Terrain.cpp

*/

#include <future>
#include <atomic>

#include "Model.h"
#include "LuaEngine.h"

using namespace darksun;

namespace darksun {

	class Terrain {

	public:
		Terrain(string mapfolder);
		~Terrain() {
			dout.log("Terrain destructor called");
		}

		bool isValid() { return valid; }

		void draw(Shader* shader);

		void tick(float deltaTime);

		bool isLoaded() {
			return loaded;
		}

		float getLoadedPercent() {
			return loadedPercent;
		}

	private:

		struct LoadingResult {
			std::vector<Texture> texts;
			std::vector<unsigned int> indiciesBuff;
			std::vector<Vertex> vertexBuff;

			int exitValue = -1;
		};

		Texture terrainText;

		std::shared_ptr<Mesh> terrainMesh;

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

		LoadingResult loadTerrain();
	};

}