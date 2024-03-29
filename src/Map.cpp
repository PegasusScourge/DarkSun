/**

File: Map.cpp
Description:

Creates a model from a height map, and textures it

THREADING IN OPERATION, might be safe

*/

#include "Map.hpp"

Map::Map(string mapfolder) {

	dout.log("Loading map \"" + mapfolder + "\"");

	// First order of business, verify the existance of the map folder and requisite files
	if (!std::filesystem::exists(mapfolder)) {
		dout.error("Attempted to create Map: map folder doesn't exist! (" + mapfolder + ")");
		return;
	}

	dir = mapfolder + "/";
	luaLoc = dir + "map.lua";

	// Attempt to load the map file
	loadingEngine.addFile(luaLoc);
	if (!loadingEngine.isValid()) {
		dout.error("Could not load map.lua of " + mapfolder);
		return;
	}

	// Check to see if the map table exists
	LuaRef mapTable = getGlobal(loadingEngine.getState()->getState(), "map");
	if (!mapTable.isTable()) {
		// It isn't a table
		dout.error("Loaded map.lua, but the map table is invalid or missing entirely (" + mapfolder + ")");
		return;
	}
	LuaRef tableLenFunc = getGlobal(loadingEngine.getState()->getState(), "tablelength");
	if (!tableLenFunc.isFunction()) {
		dout.error("Failed to find the tablelength function in the lua init file");
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
		setGammaCorrection((bool)mapTable["texture_gammaCorrection"]);
	}

	// Try to load lighting information
	if (mapTable["lighting"].isTable()) {
		LuaRef lightingTable = mapTable["lighting"];
		if (lightingTable["sun_color"].isTable()) {
			sunSpecified = true;
			// We have a sun property
			LuaRef sunClr = lightingTable["sun_color"];
			if (tableLenFunc(sunClr) == 3) {
				// There are three entries in the sun color table, thus valid
				sunColor = glm::vec3(sunClr["r"], sunClr["g"], sunClr["b"]);
			}
			else {
				dout.error("Failed to interpret lighting.sun_color table in map file: table not 3 elements long"); // Invalid table
			}
		}
		if (lightingTable["sun_position"].isTable()) {
			sunSpecified = true;
			// We have a sun property
			LuaRef sunPos = lightingTable["sun_position"];
			if (tableLenFunc(sunPos) == 3) {
				// There are three entries in the sun color table, thus valid
				sunPosition = glm::vec3(sunPos["x"], sunPos["y"], sunPos["z"]);
			}
			else {
				dout.error("Failed to interpret lighting.sun_position table in map file: table not 3 elements long"); // Invalid table
			}
		}
	}

	// Make sure we aren't marked as loaded
	setLoaded(false);

	dout.log("Spawning thread to load map....");
	loadingThreadResult = std::async(std::launch::async, &Map::loadMap, this);
	dout.log("Waiting for thread to finish...");

	//dout.log("Loaded map model and texture");
	valid = true;
}

void Map::hookClass(lua::State* L) {
	try {
		luabridge::getGlobalNamespace(L->getState())
			.beginNamespace("darksun")
				.beginClass<Map>("Map")
					.addFunction("sizeX", &darksun::Map::getSizeX)
					.addFunction("sizeY", &darksun::Map::getSizeY)
				.endClass()
			.endNamespace();

		// Add this instance
		push(L->getState(), this);
		lua_setglobal(L->getState(), "Map");
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed EntityOrders exposure proccess: " + what);
		return;
	}
}

void Map::tick(float deltaTime) {

	Renderable::tick(deltaTime);

	if (!valid) {
		dout.error("INVALID Map ERROR");
		throw new std::exception("Invalid Map");
	}
	
	if (!isLoaded()) {
		// Check to see if the result is ready
		if (loadingThreadResult._Is_ready()) {
			dout.log("Map loading finished, creating Map");
			result = loadingThreadResult.get();

			if (result.exitValue == 0) {
				// Create the mesh
				dout.verbose("MESH CREATION (Map): Got " + std::to_string(result.vertexBuff.size()) + " verticies, " + 
					std::to_string(result.indiciesBuff.size()) + " indicies");

				/*
				
				WE ARE FORCED TO LOAD THE TEXTURE IN THE MAIN THREAD TO PREVENT ANY CRAPSTORMS WITH MULTI-THREADING OPENGL
				
				*/

				std::vector<Texture> texts;
				Texture diffuse; // Create a specular map from the height map
				diffuse.id = mtopengl::getTexture(result.textInfo.diffuseSrc.c_str(), result.textInfo.diffuseGammaCorrection);
				diffuse.type = "texture_diffuse"; // Set to the diffuse
				diffuse.path = textureLoc.c_str();
				texts.push_back(diffuse);
				
				Mesh mapMesh(result.vertexBuff, result.indiciesBuff, texts);
				addMesh(mapMesh);

				setLoaded(true);
			}
			else {
				dout.error("Map loading failed!");
				valid = false;
			}
		}
	}
	else {
		// Do anything?
	}

}

// MULTI-THREADED FUNCTION
Map::LoadingResult Map::loadMap() {
	dout.log("Map loading thread launched OK");
	
	LoadingResult result;
	
	int width, height, channels;
	int wantedChannels = 1; // We want only greyscale
	dout.verbose("Map::loadMap() --> Attempting to load data from \"" + heightMapLoc + "\"");
	unsigned char *data = stbi_load(heightMapLoc.c_str(), &width, &height, &channels, wantedChannels);

	if (data == NULL) {
		// The load failed
		dout.error("stbi load failure: NULL PTR. Data could not be loaded from the image file specified: " + string(stbi_failure_reason()));
		result.exitValue = -1; // Failure exit
		return result;
	}

	loadedPercent = 10.0; // 10%

	// we got the data
	dout.verbose("Map::loadMap() --> Got " + std::to_string(channels) + " channels in image");

	if (channels != wantedChannels) {
		// We got more channels than we should have, potential unspecified behaviour afoot
		dout.warn("Loaded \"" + heightMapLoc + "\" and got more channels than exepected! (got " + std::to_string(channels) + " when we wanted " + std::to_string(wantedChannels) + ")");
	}

	dout.log("Loading heightmap data for map \"" + heightMapLoc + "\", got (" + std::to_string(width) + "," + std::to_string(height) + ") with map size of (" + std::to_string(sizeX) + "," + std::to_string(sizeY) + ")");
	
	// Calculate the conversion constant between the x and y directions
	float convX = (float)sizeX / (float)width;
	float convY = (float)sizeY / (float)height;

	dout.verbose("Map::loadMap() --> Using conversion of (" + std::to_string(convX) + "," + std::to_string(convY) + ")");

	// Create the point buffer
	std::vector<Vertex> vertexBuff;
	int heightmapBuffer_width = width;
	int heightmapBuffer_height = height;

	float percentPerIteration = 25.0f / (float)(width*height); // This moves us forward by 25%

	float lowestTemp = 100.0f;
	float highestTemp = -100.0f;

	// Loop through each y line from xn to x0 and fill the data
	for (int y = 0; y < heightmapBuffer_height; y++) {
		for (int x = 0; x < heightmapBuffer_width; x++) {
			// data is in unsigned char, 0 - 255
			float value = glm::clamp((float)data[(y*heightmapBuffer_width) + x], 0.0f, 255.0f) * 0.12;
			float textX = (float)x / (float)(heightmapBuffer_width-1);
			float textY = (float)y / (float)(heightmapBuffer_height-1);
			//dout.verbose("Coords:(" + std::to_string(x) + "," + std::to_string(y) + "), textCoords:(" + std::to_string(textX) + "," + std::to_string(textY) + ")");
			Vertex temp;
			temp.Position = glm::vec3(sizeY - (y*convY), value, x*convX);
			temp.TexCoords = glm::vec2(textX, textY);
			temp.Normal = glm::vec3(0, 1, 0);
			temp.Tangent = glm::vec3(0, 0, 1);
			temp.Bitangent = glm::vec3(1, 0, 0);

			vertexBuff.push_back(temp);

			lowestTemp = std::min(lowestTemp, value);
			highestTemp = std::max(highestTemp, value);

			loadedPercent = loadedPercent + percentPerIteration; // Keep the user updated with a loaded percent value
		}
	}

	lowestP = lowestTemp;
	highestP = highestTemp;

	dout.verbose("Map::loadMap() --> Got lowest point as: " + std::to_string(lowestP) + " with highest as: " + std::to_string(highestP));

	// Free the data buffer
	stbi_image_free(data);

	dout.verbose("Map::loadMap() --> Created and populated vertexBuff");

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

	dout.verbose("Map::loadMap() --> Created and populated indiciesBuff");

	glm::vec3 v4; glm::vec3 v2;
	glm::vec3 v1; glm::vec3 v3;
	glm::vec3 v0;

	// Do an initial smoothing pass
	float weightOthers = 0.2f;
	float weightIndiv = weightOthers / 4.0;
	float newY = 0.0f;
	for (int y = 0; y < heightmapBuffer_height; y++) {
		for (int x = 0; x < heightmapBuffer_width; x++) {
			if (x > 0 && x < heightmapBuffer_width - 1 && y > 0 && y < heightmapBuffer_height - 1) {
				Vertex *vert = &vertexBuff.at((y*heightmapBuffer_width) + x);

				// Take an average of the surrounding points and weight
				v0 = vert->Position;
				v1 = vertexBuff.at((y*heightmapBuffer_width) + x - 1).Position - v0;
				v3 = vertexBuff.at((y*heightmapBuffer_width) + x + 1).Position - v0;
				v2 = vertexBuff.at(((y + 1)*heightmapBuffer_width) + x).Position - v0;
				v4 = vertexBuff.at(((y - 1)*heightmapBuffer_width) + x).Position - v0;

				newY = (v0.y * (1.0 - weightOthers)) + (v1.y * weightIndiv) + (v2.y * weightIndiv) + (v3.y * weightIndiv) + (v4.y * weightIndiv);

				vert->Position.y = newY;
			}
		}
	}

	// Calculate the normals correctly
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

	dout.verbose("Map::loadMap() --> Perfected vertex normals (" + std::to_string(normalsProcessed) + " processed)");

	loadedPercent = 85.0f; // 85%

	// Load the texture
	ProtoTextureInfo textInfo;
	textInfo.diffuseSrc = textureLoc;
	textInfo.diffuseGammaCorrection = getGammaCorrection();

	dout.verbose("Map::loadMap() --> Texture(s) assembled");

	loadedPercent = 99.0f; // 99%

	// Mark ourselves as done
	dout.log("Map loading thread complete, exiting...");

	// Return the result
	result.textInfo = textInfo;
	result.indiciesBuff = indiciesBuff;
	result.vertexBuff = vertexBuff;
	result.exitValue = 0; // Valid exit

	return result;
}