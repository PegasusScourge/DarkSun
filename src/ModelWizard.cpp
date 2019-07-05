/**

File: ModelWizard.h
Description:

Uses Assimp to load COLLADA models for OpenGL

*/

#include "ModelWizard.h"

using namespace darksun;

bool ModelWizard::isValid() {
	return valid;
}

void ModelWizard::importScene(const std::string& file) {
	// Import and process
	const aiScene* scene = importer.ReadFile(file, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	
	if (!scene) {
		// The import failed
		dout.error("Failed to import scene " + file);
		return;
	}

	// Check for necessary elements we need in order to generate a model we can use
	// We expect textures, meshes and animations. This ensures bones, textures and meshes with indicies
	if (!scene->HasTextures() || !scene->HasMeshes() || !scene->HasAnimations()) {
		// We haven't found this
		dout.error("Scene (" + file + ") doesn't have the correct information!");
		return;
	}

	valid = processScene(scene);
}

bool ModelWizard::processScene(const aiScene* scene) {
	// We now take the scene and generate an output for the wizard to hand off
	// TODO
	return false;
}