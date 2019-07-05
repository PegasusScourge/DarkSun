#pragma once
/**

File: ModelWizard.h
Description:

Header file for ModelWizard.cpp, uses Assimp to load COLLADA models for OpenGL

*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Log.h"
#include "Model.h"

namespace darksun {
	class ModelWizard {

	private:
		Assimp::Importer importer; // The scene pointer is cleaned up by the destructor of this importer

		bool valid = false;

		/* Process the scene, returns true if scene processes correctly */
		bool processScene(const aiScene* scene);

	public:
		/* loads a scene */
		void importScene(const std::string& file);

		/* Returns if this Wizard is valid */
		bool isValid();
	};
}

