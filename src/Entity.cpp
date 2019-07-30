/**

File: Entity.cpp
Description:

A game engine actual entity, marrying together all aspects

*/

#include "Entity.hpp"

using namespace darksun;
using namespace luabridge;

/**

darksun::EntityOrders

*/

void EntityOrders::hookClass(lua::State *L) {
	try {
		luabridge::getGlobalNamespace(L->getState())
			.beginNamespace("darksun")
				.beginClass<EntityOrders>("EntityOrders")
					.addStaticProperty("ORDER_STOP", &darksun::EntityOrders::getORDER_STOP)
					.addStaticProperty("ORDER_ASSIST", &darksun::EntityOrders::getORDER_ASSIST)
					.addStaticProperty("ORDER_ATTACK", &darksun::EntityOrders::getORDER_ATTACK)
					.addStaticProperty("ORDER_MOVE", &darksun::EntityOrders::getORDER_MOVE)
					.addStaticProperty("ORDER_REPAIR", &darksun::EntityOrders::getORDER_REPAIR)
				.endClass()
			.endNamespace();
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed EntityOrders exposure proccess: " + what);
		return;
	}
}

/**

darksun::Entity

*/

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
			model->setScale((float)ref);
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
				dout.verbose("Entity::init -> Physics.hppasGravity = '" + subRef.tostring() + "'");
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
	// Register the blueprint for our entity
	engine.addFile("units/" + bpName + "/" + bpName + ".bp");

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
	lua_setglobal(L->getState(), "thisEntity");
}

void Entity::tick(float deltaTime) {
	profiler::ScopeProfiler tickProfiler("Entity.cpp::Entity::tick()");
	
	// Tick the model
	model->tick(deltaTime);
	
	if (hasScript) {
		profiler::ScopeProfiler scriptProfiler("Entity.cpp::Entity::tick()script");
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
		profiler::ScopeProfiler pathingProfiler("Entity.cpp::Entity::tick()pathfinding");
		float dist = glm::distance(model->getPosition(), pathfindingWaypoints.at(currentPathfindingWaypoint));

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
	profiler::ScopeProfiler pathingProfiler("Entity.cpp::Entity::moveOnTick()");
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
	
	float diff = glm::distance(p, model->getPosition());
	glm::vec3 v = glm::normalize(p - model->getPosition());

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
		model->setPosition(p);
		currentSpeed = 0;
	}
	else {
		//position += FORWARD * speed
		float positionx = model->getPosition().x -(speed * cosf(model->getRotation().y * (3.14f / 180.0f)));
		float positionz = model->getPosition().z + speed * sinf(model->getRotation().y * (3.14f / 180.0f));
		//dout.verbose("Tick speed: " + std::to_string(speed) + " New position: (" + std::to_string(positionx) + "," + std::to_string(positionz) + ")");
		model->setPosition(positionx, model->getPosition().y, positionz);
	}
}

void Entity::recalculatePathfinding() {
	profiler::ScopeProfiler pathingProfiler("Entity.cpp::Entity::recalculatePathfinding()");
	pathfinding = true;

	// Reset the waypoints
	pathfindingWaypoints.clear();

	// TODO make this more complicated!
	pathfindingWaypoints.push_back(targetPos);
	currentPathfindingWaypoint = 0;

	pathfindingWaypoints.shrink_to_fit();
}

void Entity::rotateToward(glm::vec3& p, float deltaTime) {
	profiler::ScopeProfiler pathingProfiler("Entity.cpp::Entity::rotateToward()");
	double dx = abs(model->getPosition().x - p.x);
	double dz = abs(model->getPosition().z - p.z);

	float theta = atan(dz / dx) * (180.0f/3.14159f);
	float angle = 0.0f;
	int quadrant = 0;
	if (p.z < model->getPosition().z) {
		if (p.x < model->getPosition().x) {
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
		if (p.x < model->getPosition().x) {
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

	if(model->getRotation().y == targetRot.y)
		return;

	float speed = maxRotSpeed * deltaTime;
	float rotDiff = targetRot.y - model->getRotation().y;
	float rotMag = abs(rotDiff);

	//if (rotMag > 90.0f) {
	//	currentSpeed = 0;
	//	readyToMove = false;
	//} 
	//else {
		readyToMove = true;
	//}

	glm::vec3 newRot = model->getRotation();

	if (rotDiff > 0.0f) {
		newRot.y += std::min(speed, rotMag);
	}
	else {
		newRot.y -= std::min(speed, rotMag);
	}

	if (newRot.y < 0.0f)
		newRot.y += 360.0f;
	if (newRot.y >= 360.0f)
		newRot.y -= 360.0f;

	model->setRotation(newRot);
}