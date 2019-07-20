#pragma once
/**

File: Model.h
Description:

Header file for Model.cpp

*/
#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Log.h"
#include "Shader.h"
#include "Renderable.h"

#include "stb_image.h"

using string = std::string;
using namespace darksun;

namespace darksun {

	class Model : public Renderable {
	public:
		/*  Model Data */
		std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		string directory = "";

		/*  Functions   */
		// constructor
		Model(string const &path, bool gamma = false) {
			this->setGammaCorrection(gamma);
			std::filesystem::path p2 = std::filesystem::absolute(path);
			string p = p2.u8string();
			//dout.log("Load model: " + p);
			loadModel(p);
		}

		// MUST HAVE A GENERIC FORWARD SLASHED PATH! Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void loadModel(string const &path);

		unsigned int TextureFromFile(const char * path, const string &directory, bool gamma);
	private:
		/*  Functions   */

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(aiNode *node, const aiScene *scene);

		Mesh processMesh(aiMesh *mesh, const aiScene *scene);

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	};

}