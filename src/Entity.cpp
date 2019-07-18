/**

File: Entity.cpp
Description:

A game engine actual entity, marrying together all aspects

*/

#include "Entity.h"

using namespace darksun;
using namespace luabridge;

void EntityOrders::hookClass(lua::State *L) {
	try {
		luabridge::getGlobalNamespace(L->getState())
			.beginNamespace("darksun")
				.beginClass<EntityOrders>("EntityOrders")
					.addStaticFunction("ORDER_STOP", &darksun::EntityOrders::getORDER_STOP)
					.addStaticFunction("ORDER_ASSIST", &darksun::EntityOrders::getORDER_ASSIST)
					.addStaticFunction("ORDER_ATTACK", &darksun::EntityOrders::getORDER_ATTACK)
					.addStaticFunction("ORDER_MOVE", &darksun::EntityOrders::getORDER_MOVE)
					.addStaticFunction("ORDER_REPAIR", &darksun::EntityOrders::getORDER_REPAIR)
				.endClass()
			.endNamespace();
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed EntityOrders exposure proccess: " + what);
		return;
	}
}

int Entity::LastEntityId = 0;

int Entity::createNewId() {
	// Return a new numerical Id
	return ++LastEntityId;
}

Entity::Entity(string blueprintn, int newId) {
	init(blueprintn, newId);
}

void Entity::init(string blueprintn, int newId) {
	dout.log("Creating new entity with id '" + to_str(newId) + "'");
	
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

		// Physics
		ref = myBp["Physics"];
		if (ref.isTable()) {
			// We have physics defs!
			LuaRef subRef = ref["maxSpeed"];
			if (subRef.isNumber()) {
				moveSpeed = (float)subRef;
				dout.verbose("Entity::init -> Physics.maxSpeed = '" + subRef.tostring() + "'");
			}
			subRef = ref["acceleration"];
			if (subRef.isNumber()) {
				acceleration = (float)subRef;
				dout.verbose("Entity::init -> Physics.acceleration = '" + subRef.tostring() + "'");
			}
			subRef = ref["hasGravity"];
			if (subRef.isBool()) {
				hasGravity = (bool)subRef;
				dout.verbose("Entity::init -> Physics.hasGravity = '" + subRef.tostring() + "'");
			}
			subRef = ref["size"];
			if (subRef.isTable()) {
				if(subRef["x"].isNumber() && subRef["y"].isNumber() && subRef["z"].isNumber()){
					size = glm::vec3((float)subRef["x"], (float)subRef["y"], (float)subRef["z"]);
					dout.verbose("Entity::init -> Physics.size = '(" + subRef["x"].tostring()  + "," + subRef["y"].tostring() + "," + subRef["z"].tostring() + ")'");
				}
				else {
					dout.warn("Entity::init -> Found size table in blueprint '" + bpName + "' but it has invalid values");
				}
			}
		}

		// Script
		ref = myBp["Script"];
		if (ref.isString()) {
			// We have a script file, load it
			dout.verbose("Entity::init -> Script = '" + ref.tostring() + "'");
			engine.addFile(ref);
			hasScript = true;

			// Call on create
			lua::State *L = engine.getState();
			try {
				LuaRef entityTable = getGlobal(L->getState(), internalName.c_str());
				if (!entityTable.isTable()) // Check to see if the scene table exists
					throw new std::exception("Entity table global not found");
				LuaRef func = entityTable["OnCreate"];
				if (!func.isFunction())
					throw new std::exception("OnCreate function not found/not a function");
				func();
			}
			catch (std::exception& e) {
				string what = e.what();
				dlua.error("Failed entity OnCreate (" + internalName + "): " + what);
			}
		}
		else {
			dout.warn("No/invalid script found for blueprint '" + bpName + "', won't attempt to run a script");
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

	// Register the entity class
	lua::State *L = engine.getState();
	luabridge::getGlobalNamespace(L->getState())
		.beginNamespace("darksun")
			.beginClass<Entity>("Entity")
				//.addConstructor<void(*)(string blueprintn, int newId), RefCountedPtr<Entity> /* creation policy */ >()
				.addProperty("internalName", &darksun::Entity::internalName, false) // Read only
				.addProperty("bpName", &darksun::Entity::bpName, false) // Read only
				.addProperty("id", &darksun::Entity::myId, false) // Read only
				.addFunction("moveTo", &darksun::Entity::setMoveTarget)
				.addFunction("rotateTo", &darksun::Entity::faceDirection)
				.addFunction("isPathfinding", &darksun::Entity::isPathfinding)
			.endClass()
		.endNamespace();

	// Hook classes
	EntityOrders::hookClass(engine.getState());

	// Add this instance
	push(L->getState(), this);
	lua_setglobal(L->getState(), "myEntity");
}

void Entity::draw(Shader* shader, bool drawReflection, bool reflectiveSurface) {
	if (!valid) {
		dout.error("ATTEMPTED TO DRAW INVALID ENTITY?!");
		return;
	}

	// render the loaded model
	glm::mat4 modelm = glm::mat4(1.0f);
	modelm = glm::translate(modelm, position);
	modelm = glm::scale(modelm, scale);
	modelm = glm::rotate(modelm, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //X
	modelm = glm::rotate(modelm, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //Y
	modelm = glm::rotate(modelm, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //Z
	shader->setMat4("model", modelm);

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
		if (!reflectiveSurface) // If we are a reflective surface, stencil buffer is already enabled
			glEnable(GL_STENCIL_TEST);

		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

		// Reflection
		glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
		glStencilMask(0x00); // Don't write anything to stencil buffer
		glDepthMask(GL_TRUE); // Write to depth buffer

		modelm = glm::mat4(1.0f);
		modelm = glm::scale(glm::translate(modelm, glm::vec3(position.x, position.y * -1.0f, position.z)), scale * -1.0f);
		modelm = glm::rotate(modelm, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //X
		modelm = glm::rotate(modelm, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //Y
		modelm = glm::rotate(modelm, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //Z
		shader->setMat4("model", modelm);

		// Draw the reflection
		model->draw(shader);
	}

	if (reflectiveSurface || drawReflection) // Disable the stencil buffer
		glDisable(GL_STENCIL_TEST);
}

void Entity::tick(float deltaTime) {
	if (hasScript) {
		lua::State *L = engine.getState();
		try {
			LuaRef entityTable = getGlobal(L->getState(), internalName.c_str());
			if (!entityTable.isTable()) // Check to see if the scene table exists
				throw new std::exception("Entity table global not found");
			LuaRef onTick = entityTable["OnTick"];
			if (!onTick.isFunction())
				throw new std::exception("OnTick function not found/not a function");
			onTick(deltaTime);
		}
		catch (std::exception& e) {
			string what = e.what();
			dlua.error("Failed entity OnTick (" + internalName + "): " + what);
		}
	}

	// Do movement stuff

	if (pathfinding) {
		float dist = glm::distance(position, pathfindingWaypoints.at(currentPathfindingWaypoint));

		rotateToward(pathfindingWaypoints.at(currentPathfindingWaypoint), deltaTime);
		if (dist > 0.001) {
			moveOnTick(pathfindingWaypoints.at(currentPathfindingWaypoint), deltaTime);
		}
		else {
			pathfinding = false;
		}
	}
}

void Entity::moveOnTick(glm::vec3& p, float deltaTime) {
	if (!readyToMove) {
		if (currentSpeed > 0.0f) {
			currentSpeed -= std::min(acceleration * deltaTime, currentSpeed); // Come to a stop
		}
		else {
			currentSpeed -= acceleration * deltaTime; // Back up slowly
			if (currentSpeed < moveSpeed * -0.33f) {
				currentSpeed = moveSpeed * -0.33f;
			}
		}
		return;
	}
	
	float diff = glm::distance(p, position);
	glm::vec3 v = glm::normalize(p - position);

	// Made possible with the help of DaShoup
	float s = pow(currentSpeed, 2) / (acceleration * 2);

	if (diff < s) {
		// decelerate
		currentSpeed -= acceleration * deltaTime;
	}
	else {
		// accelerate
		currentSpeed += acceleration * deltaTime;
	}

	if (currentSpeed < 0.0f)
		currentSpeed = 0.0f;
	if (currentSpeed > moveSpeed)
		currentSpeed = moveSpeed;

	float speed = currentSpeed * deltaTime;
	//dout.verbose("Current speed: " + std::to_string(currentSpeed) + ", stopping distance: " + std::to_string(s) + ", dist: " + std::to_string(diff));

	if (diff < speed) {
		position = p;
		currentSpeed = 0;
	}
	else {
		//position += FORWARD * speed;
		position.x -= speed * cosf(rotation.y * (3.14f / 180.0f));
		position.z += speed * sinf(rotation.y * (3.14f / 180.0f));
	}
}

void Entity::recalculatePathfinding() {
	pathfinding = true;

	// Reset the waypoints
	pathfindingWaypoints.clear();

	// TODO make this more complicated!
	pathfindingWaypoints.push_back(targetPos);
	currentPathfindingWaypoint = 0;

	pathfindingWaypoints.shrink_to_fit();
}

void Entity::rotateToward(glm::vec3& p, float deltaTime) {
	double dx = abs(position.x - p.x);
	double dz = abs(position.z - p.z);

	float theta = atan(dz / dx) * (180.0f/3.14159f);
	float angle = 0.0f;
	int quadrant = 0;
	if (p.z < position.z) {
		if (p.x < position.x) {
			// -z +x
			angle = 360.0f - angle;
			quadrant = 4;
		}
		else {
			// -z -x
			angle = 180.0f + theta;
			quadrant = 3;
		}
	}
	else {
		if (p.x < position.x) {
			// +z +x
			angle = theta;
			quadrant = 1;
		}
		else {
			// +z -x
			angle = 180.0f - theta;
			quadrant = 2;
		}
	}

	//dout.verbose("Angle: " + to_str(angle) + " Quadrant: " + to_str(quadrant));

	targetRot.y = angle;

	if(rotation.y == targetRot.y)
		return;

	float speed = maxRotSpeed * deltaTime;
	float rotDiff = targetRot.y - rotation.y;
	float rotMag = abs(rotDiff);

	if (rotMag > 90.0f) {
		currentSpeed = 0;
		readyToMove = false;
	} 
	else {
		readyToMove = true;
	}

	if (rotDiff > 0.0f) {
		rotation.y += std::min(speed, rotMag);
	}
	else {
		rotation.y -= std::min(speed, rotMag);
	}

	if (rotation.y < 0.0f)
		rotation.y += 360.0f;
	if (rotation.y >= 360.0f)
		rotation.y -= 360.0f;
}