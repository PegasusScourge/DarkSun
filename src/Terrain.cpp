/**

File: Terrain.cpp
Description:

Creates a model from a height map, and textures it

*/

#include "Terrain.h"

Terrain::Terrain(string mapfolder) {

	dout.log("Loading map \"" + mapfolder + "\"");

	// First order of business, verify the existance of the map folder and requisite files
	if (!std::filesystem::exists(mapfolder)) {
		dout.error("Attempted to create terrain: map folder doesn't exist! (" + mapfolder + ")");
		return;
	}

	dir = mapfolder + "/";
	luaLoc = dir + "map.lua";

	// Attempt to load the map file
	engine.addFile(luaLoc);
	if (!engine.isValid()) {
		dout.error("Could not load map.lua of " + mapfolder);
		return;
	}

	// Check to see if the map table exists
	LuaRef mapTable = getGlobal(engine.getState()->getState(), "map");
	if (!mapTable.isTable()) {
		// It isn't a table
		dout.error("Loaded map.lua, but the map table is invalid or missing entirely (" + mapfolder + ")");
		return;
	}

	if (!mapTable["heightmap"].isString() || !mapTable["texture"].isString()) {
		// These entries must exist for the map to be valid
		dout.error("Loaded map.lua, but the map info table is invalid (" + mapfolder + ")");
		return;
	}

	heightMapLoc = dir + mapTable["heightmap"].tostring();
	textureLoc = dir + mapTable["texture"].tostring();
	sizeX = 1024; sizeY = 1024; // Set the default values for the size of the map

	// Try to load physics data for the map if it exists, not the end of the world if it doesn't
	if (mapTable["physics"].isTable()) {
		LuaRef physTable = mapTable["physics"];
		if (physTable["sizeX"].isNumber()) {
			sizeX = (int)physTable["sizeX"];
		}
		if (physTable["sizeY"].isNumber()) {
			sizeY = (int)physTable["sizeY"];
		}
	}

	// Try to load gammaCorrection info
	if (mapTable["texture_gammaCorrection"].isBool()) {
		gammaCorrection = (bool)mapTable["texture_gammaCorrection"];
	}

	// Make sure we aren't marked as loaded
	loaded = false;

	dout.log("Spawning thread to load map....");
	loadingThreadResult = std::async(std::launch::async, &Terrain::loadTerrain, this);
	dout.log("Waiting for thread to finish...");

	//dout.log("Loaded map model and texture");
	valid = true;
}

void Terrain::tick(float deltaTime) {

	if (!valid) {
		dout.error("INVALID TERRAIN ERROR");
		throw new std::exception("Invalid terrain");
	}
	
	if (!isLoaded()) {
		// Check to see if the result is ready
		if (loadingThreadResult._Is_ready()) {
			dout.log("Terrain loading finished, creating terrain");
			result = loadingThreadResult.get();

			if (result.exitValue == 0) {
				// Create the mesh
				dout.verbose("MESH CREATION (Terrain): Got " + std::to_string(result.vertexBuff.size()) + " verticies, " + 
					std::to_string(result.indiciesBuff.size()) + " indicies");

				/*
				
				WE ARE FORCED TO LOAD THE TEXTURE IN THE MAIN THREAD TO PREVENT ANY CRAPSTORMS WITH MULTI-THREADING OPENGL
				
				*/

				std::vector<Texture> texts;
				Texture diffuse; // Create a specular map from the height map
				diffuse.id = TextureFromFile(result.textInfo.diffuseSrc.c_str(), result.textInfo.diffuseGammaCorrection);
				diffuse.type = "texture_diffuse"; // Set to the diffuse
				diffuse.path = textureLoc.c_str();
				texts.push_back(diffuse);
				
				terrainMesh = std::unique_ptr<Mesh>(new Mesh(result.vertexBuff, result.indiciesBuff, texts));
				loaded = true;
			}
			else {
				dout.error("Terrain loading failed!");
				valid = false;
			}
		}
	}
	else {
		// Do anything?
	}

}

void Terrain::draw(Shader* shader) {
	if (!valid) {
		dout.error("INVALID TERRAIN ERROR");
		throw new std::exception("Invalid terrain");
	}
	
	shader->setInt("gamma", gammaCorrection);

	if (!isLoaded()) {
		dout.error("ATTEMPTED TO DRAW TERRAIN BEFORE IT IS LOADED");
	}
	//dout.verbose("Passed loading check");

	glm::mat4 modelm = glm::mat4(1.0f); // Move the terrain to the normal position
	modelm = glm::translate(modelm, glm::vec3(0, 0, 0));
	modelm = glm::scale(modelm, glm::vec3(1, 1, 1));
	modelm = glm::rotate(modelm, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //X
	modelm = glm::rotate(modelm, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Y
	modelm = glm::rotate(modelm, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //Z
	shader->setMat4("model", modelm);

	shader->setVec3("objectColor", glm::vec3(1, 1, 1));

	terrainMesh->draw(shader);
}

// MULTI-THREADED FUNCTION
Terrain::LoadingResult Terrain::loadTerrain() {
	dout.log("Terrain loading thread launched OK");
	
	LoadingResult result;
	
	int width, height, channels;
	int wantedChannels = 1; // We want only greyscale
	dout.verbose("Terrain::loadTerrain() --> Attempting to load data from \"" + heightMapLoc + "\"");
	unsigned char *data = stbi_load(heightMapLoc.c_str(), &width, &height, &channels, wantedChannels);

	if (data == NULL) {
		// The load failed
		dout.error("stbi load failure: NULL PTR. Data could not be loaded from the image file specified: " + string(stbi_failure_reason()));
		result.exitValue = -1; // Failure exit
		return result;
	}

	loadedPercent = 10.0; // 10%

	// we got the data
	dout.verbose("Terrain::loadTerrain() --> Got " + std::to_string(channels) + " channels in image");

	if (channels != wantedChannels) {
		// We got more channels than we should have, potential unspecified behaviour afoot
		dout.warn("Loaded \"" + heightMapLoc + "\" and got more channels than exepected! (got " + std::to_string(channels) + " when we wanted " + std::to_string(wantedChannels) + ")");
	}

	dout.log("Loading heightmap data for map \"" + heightMapLoc + "\", got (" + std::to_string(width) + "," + std::to_string(height) + ") with map size of (" + std::to_string(sizeX) + "," + std::to_string(sizeY) + ")");
	
	// Calculate the conversion constant between the x and y directions
	float convX = (float)sizeX / (float)width;
	float convY = (float)sizeY / (float)height;

	dout.verbose("Terrain::loadTerrain() --> Using conversion of (" + std::to_string(convX) + "," + std::to_string(convY) + ")");

	// Create the point buffer
	std::vector<Vertex> vertexBuff;
	int heightmapBuffer_width = width;
	int heightmapBuffer_height = height;

	float percentPerIteration = 25.0f / (float)(width*height); // This moves us forward by 25%

	// Loop through each y line from x0 to xn and fill the data
	for (int y = 0; y < heightmapBuffer_height; y++) {
		for (int x = 0; x < heightmapBuffer_width; x++) {
			// data is in unsigned char, 0 - 255
			float value = (float)data[(y*heightmapBuffer_width) + x] * glm::clamp(std::min(convX * 2, convY * 2), 0.1f, 1.0f);
			float textX = (float)x / (float)(heightmapBuffer_width-1);
			float textY = (float)y / (float)(heightmapBuffer_height-1);
			//dout.verbose("Coords:(" + std::to_string(x) + "," + std::to_string(y) + "), textCoords:(" + std::to_string(textX) + "," + std::to_string(textY) + ")");
			Vertex temp;
			temp.Position = glm::vec3(x*convX, value, y*convY);
			temp.TexCoords = glm::vec2(textX, textY);
			temp.Normal = glm::vec3(0, 1, 0);
			temp.Tangent = glm::vec3(0, 0, 1);
			temp.Bitangent = glm::vec3(1, 0, 0);

			vertexBuff.push_back(temp);

			loadedPercent = loadedPercent + percentPerIteration; // Keep the user updated with a loaded percent value
		}
	}

	// Free the data buffer
	stbi_image_free(data);

	dout.verbose("Terrain::loadTerrain() --> Created and populated vertexBuff");

	// Create the indicies
	std::vector<unsigned int> indiciesBuff;
	for (int y = 0; y < heightmapBuffer_height - 1; y++) {
		for (int x = 0; x < heightmapBuffer_width - 1; x++) {
			unsigned int topL = (y*heightmapBuffer_width) + x;
			unsigned int topR = topL + 1;
			unsigned int botL = ((y + 1)*heightmapBuffer_width) + x;
			unsigned int botR = botL + 1;

			// Do first triangle
			indiciesBuff.push_back(botL); indiciesBuff.push_back(topR); indiciesBuff.push_back(topL); // Correct
			//indiciesBuff.push_back(topL); indiciesBuff.push_back(topR); indiciesBuff.push_back(botL); // Old way

			// Do second triangle
			indiciesBuff.push_back(botL); indiciesBuff.push_back(botR); indiciesBuff.push_back(topR); // Correct
			//indiciesBuff.push_back(topR); indiciesBuff.push_back(botR); indiciesBuff.push_back(botL); // Old way

			loadedPercent = loadedPercent + percentPerIteration; // Keep the user updated with a loaded percent value
		}
	}

	dout.verbose("Terrain::loadTerrain() --> Created and populated indiciesBuff");

	// Calculate the normals correctly
	glm::vec3 v4; glm::vec3 v2;
	glm::vec3 v1; glm::vec3 v3;
	glm::vec3 v0;
	glm::vec3 v12; glm::vec3 v23;
	glm::vec3 v34; glm::vec3 v41;
	int normalsProcessed = 0;
	for (int y = 0; y < heightmapBuffer_height; y++) {
		for (int x = 0; x < heightmapBuffer_width; x++) {
			if (x > 0 && x < heightmapBuffer_width - 1 && y > 0 && y < heightmapBuffer_height - 1) {
				Vertex *vert = &vertexBuff.at((y*heightmapBuffer_width) + x);

				v0 = vert->Position;
				v1 = vertexBuff.at((y*heightmapBuffer_width) + x - 1).Position - v0;
				v3 = vertexBuff.at((y*heightmapBuffer_width) + x + 1).Position - v0;
				v2 = vertexBuff.at(((y+1)*heightmapBuffer_width) + x).Position - v0;
				v4 = vertexBuff.at(((y-1)*heightmapBuffer_width) + x).Position - v0;

				v12 = glm::normalize(glm::cross(v1, v2));
				v23 = glm::normalize(glm::cross(v2, v3));
				v34 = glm::normalize(glm::cross(v3, v4));
				v41 = glm::normalize(glm::cross(v4, v1));

				vert->Normal = glm::normalize(v12 + v23 + v34 + v41);
				vert->Bitangent = glm::normalize(glm::cross(vert->Normal, vert->Tangent));

				normalsProcessed++;
			}
			// The normals are initialised fine already so we can just ignore if we don't meet the above conditions

			loadedPercent = loadedPercent + percentPerIteration; // Keep the user updated with a loaded percent value
		}
	}

	dout.verbose("Terrain::loadTerrain() --> Perfected vertex normals (" + std::to_string(normalsProcessed) + " processed)");

	loadedPercent = 85.0f; // 85%

	// Load the texture
	ProtoTextureInfo textInfo;
	textInfo.diffuseSrc = textureLoc;
	textInfo.diffuseGammaCorrection = gammaCorrection;

	dout.verbose("Terrain::loadTerrain() --> Texture(s) assembled");

	loadedPercent = 99.0f; // 99%

	// Mark ourselves as done
	dout.log("Terrain loading thread complete, exiting...");

	// Return the result
	result.textInfo = textInfo;
	result.indiciesBuff = indiciesBuff;
	result.vertexBuff = vertexBuff;
	result.exitValue = 0; // Valid exit

	return result;
}

unsigned int Terrain::TextureFromFile(const string filename, bool gamma) {

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		dout.error("Texture failed to load at path: " + filename);
		stbi_image_free(data);
	}

	return textureID;
}