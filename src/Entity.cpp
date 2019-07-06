/**

File: Entity.cpp
Description:

A game engine actual entity, marrying together all aspects

*/

#include "Entity.h"

using namespace darksun;
using namespace luabridge;

long Entity::LastEntityId = 0;

long Entity::createNewId() {
	// Return a new numerical Id
	return ++LastEntityId;
}

Entity::Entity(string blueprintn, long newId) {
	init(blueprintn, newId);
}

void Entity::init(string blueprintn, long newId) {
	dout.log("Creating new entity with id '" + to_string(newId) + "'");
	
	myId = newId;
	bpName = blueprintn;

	initLuaEngine();

	// Check to see if the blueprints table exists
	LuaRef bpTable = getGlobal(engine.getState()->getState(), "blueprints");

	if (!bpTable.isTable()) {
		dlua.error("'blueprints' table is not a table, or is not defined!");
		dlua.log("The fact this is even possible is incredible");
		return;
	}

	// Load our sub-table, with our blueprint info
	LuaRef myBp = bpTable[bpName.c_str()];
	if (!myBp.isTable()) {
		dlua.error("Got non-table LuaRef when loading blueprint");
		dlua.log("Does the blueprint get declared in a table correctly?");
		return;
	}

	try {
		/****** Load information from the blueprint ******/
		// Top level information
		if (myBp["InternalName"].isString()) {
			internalName = myBp["InternalName"].tostring();
			dout.verbose("Entity::init -> InternalName = '" + internalName + "'");
		}
		else {
			// We need a valid string InternalName to function, fail the entity here
			dout.error("ENTITY LOAD ERROR: No valid InternalName for blueprint '" + bpName + "' (must be present and of string type)");
			return;
		}
		
		// Model
		LuaRef modelInf = myBp["Model"]; // Get the sub-table
		if (!modelInf.isTable()) {
			dlua.error("Got non-table LuaRef when loading blueprint, 'Model' sub-table not found");
			return;
		}
		//dout.verbose("Entity::init -> Model sub-table found");
		
		LuaRef ref = modelInf["lod_0"];
		if (ref.isString()) {
			
			//std::filesystem::path p = std::filesystem::absolute(ref.tostring());
			std::filesystem::path p(std::filesystem::current_path().generic_string() + "/" + ref.tostring());
			if (std::filesystem::exists(p)) {
				dout.verbose("Entity::init -> Model.lod_0 = '" + p.generic_string() + "'");
				model = std::shared_ptr<Model>(new Model(p.generic_string())); // Close-range, high detail model
				dout.verbose("Entity::init -> Model.lod_0 loaded");
			}
			else {
				dout.error("ENTITY LOAD ERROR: No valid base model/mesh to display with (bp = '" + bpName + "', '" + p.generic_string() + "' doesNotExist)");
				return;
			}
		}
		else {
			dout.error("ENTITY LOAD ERROR: No valid base model/mesh to display (bp = '" + bpName + "', isNotString)");
			return;
		}

		ref = modelInf["UniformScale"];
		if (ref.isNumber()) {
			// We have the field UniformScale, extract
			// Set the scale uniformly
			dout.verbose("Entity::init -> Model.UniformScale = '" + ref.tostring() + "'");
			scale.x = (float)ref;
			scale.y = (float)ref;
			scale.z = (float)ref;
		}
		else {
			dout.warn("No/invalid scale information for blueprint '" + bpName + "'");
		}

		// Script
		ref = myBp["Script"];
		if (ref.isString()) {
			// We have a script file, load it
			dout.verbose("Entity::init -> Script = '" + ref.tostring() + "'");
			engine.addFile(ref);
		}
		else {
			dout.warn("No/invalid script found for blueprint '" + bpName + "'");
		}

		// Anything else?
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Error on loading entity data from blueprint: '" + what + "'. This is likely due to malformations or lack of declaration.");
		return;
	}

	// We survived the process, mark ourselves as being done (we hope)
	valid = true;
}

void Entity::initLuaEngine() {
	// Register the blueprints for our entity
	engine.addFilesRecursive("units/", ".bp");

	// Register std lua here?
	engine.addFilesRecursive("lua/std/", ".lua");
}

void Entity::draw(Shader& shader, bool drawReflection, bool reflectiveSurface) {
	if (!valid) {
		dout.error("ATTEMPTED TO DRAW INVALID ENTITY?!");
		return;
	}
	
	// render the loaded model
	glm::mat4 modelm = glm::mat4(1.0f);
	modelm = glm::translate(modelm, position); // translate it down so it's at the center of the scene
	modelm = glm::scale(modelm, scale);	// it's a bit too big for our scene, so scale it down
	modelm = glm::rotate(modelm, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //X
	modelm = glm::rotate(modelm, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //Y
	modelm = glm::rotate(modelm, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //Z
	shader.setMat4("model", modelm);
	
	if (reflectiveSurface) {
		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
	}

	model->draw(shader);

	if (drawReflection) {
		if(!reflectiveSurface) // If we are a reflective surface, stencil buffer is already enabled
			glEnable(GL_STENCIL_TEST);

		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

		// Reflection
		glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
		glStencilMask(0x00); // Don't write anything to stencil buffer
		glDepthMask(GL_TRUE); // Write to depth buffer

		modelm = glm::mat4(1.0f);
		modelm = glm::scale(glm::translate(modelm, glm::vec3(position.x, position.y * -1.0f, position.z)),scale * -1.0f);
		modelm = glm::rotate(modelm, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //X
		modelm = glm::rotate(modelm, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //Y
		modelm = glm::rotate(modelm, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //Z
		shader.setMat4("model", modelm);
		
		// Draw the reflection
		model->draw(shader);
	}

	if (reflectiveSurface || drawReflection) // Disable the stencil buffer
		glDisable(GL_STENCIL_TEST);
}
