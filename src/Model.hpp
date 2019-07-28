#pragma once
/**

File: Model.hpp
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

#include "Log.hpp"
#include "Shader.hpp"
#include "Renderable.hpp"
#include "MultiThreadedOpenGL.hpp"

using string = std::string;
using namespace darksun;

namespace darksun {

	class Model : public Renderable {
	public:
		/*  Model Data */
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